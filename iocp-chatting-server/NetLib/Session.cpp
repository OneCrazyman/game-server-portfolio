#include "stdafx.h"
#include "Session.h"

#include "define.h"
#include "Util/CPacket.h"
#include "Util/SystemLogger.h"
#include "SessionManager.h"
#include "Packet/PacketProcess_Auto.h"

#define ENABLED_PROFILER
#include "Util/Profiler.h"

using enum SystemLogger::LOG_LEVEL;

void Session::Init(SOCKET clientSocket, uint32_t id, SOCKADDR_IN& clientaddr, SessionConfig& sessionInfo)
{
	WCHAR ip[INET_ADDRSTRLEN];
	InetNtop(AF_INET, &clientaddr.sin_addr, ip, _countof(ip));
	u_short port = ntohs(clientaddr.sin_port);

	std::unique_lock<std::mutex> lock(mutex_);

	id_ = id;
	gen_ = genCount_++;
	state_ = STATE::ACTIVE;
	socket_ = clientSocket;
	ip_ = ip;
	port_ = port;
	recvQ_.ClearBuffer();
	sendQ_.ClearBuffer();
	ZeroMemory(&overlappedRecv_, sizeof(overlappedRecv_));
	ZeroMemory(&overlappedSend_, sizeof(overlappedSend_));
	overlappedRecv_.type = IO_TYPE::IO_RECV;
	overlappedSend_.type = IO_TYPE::IO_SEND;
	disconnectRequested_ = false;
	userId_ = 0;

	lock.unlock();

	SLog(DEBUG_LEVEL,L"[TCP server] client connect: ip = %ls, port = %d\n", ip_.c_str(), port_);
}

void Session::ClearLock()
{
	std::lock_guard<std::mutex> lock(mutex_);
	this->Clear();
}

void Session::Clear()
{
	state_ = STATE::INACTIVE;
}

bool Session::PostRecv()
{
	DWORD flags = 0;
	DWORD recvbytes = 0;
	WSABUF buf[2] = {};

	std::unique_lock<std::mutex> lock(mutex_);
	if (!this->IsActive()) {
		return false;
	}
	this->IncrementRefCount();
	lock.unlock();

	// 락 내부에서 세션의 활성 여부 체크 및 릴리즈 방지로 락없이 진행 가능
	buf[0].buf = recvQ_.GetRearBufferPtr();
	buf[0].len = recvQ_.DirectEnqueueSize();
	buf[1].buf = recvQ_.GetBufferPtr();
	buf[1].len = recvQ_.GetFreeSize() - buf[0].len;
	int retRecv = WSARecv(socket_, buf, 2, &recvbytes, &flags, (WSAOVERLAPPED*)&overlappedRecv_, NULL);
	if (retRecv == SOCKET_ERROR) {
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING) {
			SLog(ERROR_LEVEL, L"recv(%d)",error);
			this->DecrementRefCountAndRelease();
			return false;
		}
	}
	else {
		SLog(DEBUG_LEVEL, L"recv: not io pending");
	}
	return true;
}

bool Session::PostSend()
{
	std::unique_lock<std::mutex> lock(mutex_);
	if (state_ == STATE::INACTIVE) {
		return false;
	}
	if (isSending_) {
		return false;
	}
	this->IncrementRefCount();

	isSending_ = true;

	WSABUF buf[2] = {};
	DWORD sendbytes = 0;
	buf[0].buf = sendQ_.GetFrontBufferPtr();
	buf[0].len = sendQ_.DirectDequeueSize();
	buf[1].buf = sendQ_.GetBufferPtr();
	buf[1].len = sendQ_.GetUseSize() - buf[0].len;
	lock.unlock();

	int retSend = WSASend(socket_, buf, 2, &sendbytes, 0, (WSAOVERLAPPED*)&overlappedSend_, NULL);
	if (retSend == SOCKET_ERROR) {
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING) {
			SLog(ERROR_LEVEL, L"send (%d)", error);
			this->DecrementRefCountAndRelease();
			return false;
		}
	}
	else {
		SLog(ERROR_LEVEL, L"send: not io pending");
	}
	return true;
}

void Session::OnRecv(DWORD transferred)
{
	SLog(DEBUG_LEVEL, L"Recvd: %d bytes",transferred);

	int recved = recvQ_.MoveRear(transferred);
	if (recved == 0) {
		SLog(ERROR_LEVEL, L"recvQ MoveRear failed");
		this->DecrementRefCountAndRelease();
		return;
	}

	int64_t msg = NULL;
	while (1) {
		if (recvQ_.GetUseSize() > sizeof(PktHeader)) {
			PktHeader header;
			recvQ_.Peek((char*)&header, sizeof(PktHeader));
			if (!CheckHeader(header)) {
				this->RequestDisconnect();
				break;
			}
			if (recvQ_.GetUseSize() >= sizeof(PktHeader) + header.Len) {
				recvQ_.MoveFront(sizeof(PktHeader));

				CPacket pkt;
				auto retDeq = recvQ_.Dequeue(pkt.GetBufferPtr(), header.Len);
				if (retDeq != header.Len) {
					SLog(ERROR_LEVEL, L"header len");
				}
				pkt.MoveWritePos(header.Len);
				OnMessage(0, pkt);
			}
			else break;
		}
		else break;
	}

	bool released = this->DecrementRefCount();
	if (released) {
		return;
	}
	else {
		this->PostRecv();
	}
}

void Session::OnSend(DWORD transferred)
{
	SLog(DEBUG_LEVEL, L"sended: %d bytes", transferred);
	bool released = this->DecrementRefCount();
	if (released) {
		return;
	}
	std::unique_lock<std::mutex> lock(mutex_);
	if (!this->IsActive()) {
		return;
	}
	sendQ_.MoveFront(transferred);
	isSending_ = false;
	if (sendQ_.GetUseSize() > 0) {
		lock.unlock();
		this->PostSend();
	}
}

void Session::OnMessage(uint16_t type, CPacket& packet) const
{
	PacketProcess_Auto::Instance().Process(type, id_, packet);
}

void Session::SendPacket(CPacket& packet)
{
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (state_ == STATE::INACTIVE) {
			return;
		}
		int put = sendQ_.Enqueue(packet.GetBufferPtr(), packet.GetDataSize());
		if (put != packet.GetDataSize()) {
			SLog(ERROR_LEVEL, L"SendQ is full");
			this->RequestDisconnect();
			return;
		}
	}
	this->PostSend();
}

void Session::IncrementRefCount()
{
	++refCount;
}

bool Session::DecrementRefCount()
{
   std::unique_lock<std::mutex> lock(mutex_);
   auto count = --refCount;
   if (disconnectRequested_ && count < 1) {
       SessionManager::Instance().DisconnectSession(id_, std::move(lock));
       return true;
   }
   return false;
}

void Session::DecrementRefCountAndRelease()
{
	std::unique_lock<std::mutex> lock(mutex_);
	auto count = --refCount;
	if (count < 1) {
		SessionManager::Instance().DisconnectSession(id_,std::move(lock));
	}
	else {
		this->RequestDisconnect();
	}
}

bool Session::CheckHeader(PktHeader& header)
{
	// TODO
	return true;
}