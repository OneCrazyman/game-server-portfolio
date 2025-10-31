#include "stdafx.h"
#include "Session.h"

#include "define.h"
#include "CPacket.h"
#include "SystemLogger.h"
#include "SessionManager.h"
#include "PacketProcess_Auto.h"

using enum SystemLogger::LOG_LEVEL;

void Session::Init(SOCKET clientSocket, uint32_t id, SOCKADDR_IN& clientaddr, SessionInfo& sessionInfo)
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
	this->CloseSocket();
	state_ = STATE::INACTIVE;
}

void Session::Clear()
{
	this->CloseSocket();
	state_ = STATE::INACTIVE;
}

bool Session::PostRecv()
{
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (state_ == STATE::INACTIVE) {
			return false;
		}
	}

	DWORD flags = 0;
	DWORD recvbytes = 0;
	WSABUF buf[2] = {};
	buf[0].buf = recvQ_.GetRearBufferPtr();
	buf[0].len = recvQ_.DirectEnqueueSize();
	buf[1].buf = recvQ_.GetBufferPtr();
	buf[1].len = recvQ_.GetFreeSize() - buf[0].len;
	this->IncrementRefCount();
	int retRecv = WSARecv(socket_, buf, 2, &recvbytes, &flags, (WSAOVERLAPPED*)&overlappedRecv_, NULL);
	if (retRecv == SOCKET_ERROR) {
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING) {
			SLog(ERROR_LEVEL, L"recv(%d)",error);
			this->DecrementRefCountAndReleaseLock();
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
	std::lock_guard<std::mutex> lock(mutex_);

	if (state_ == STATE::INACTIVE) {
		return false;
	}

	if (isSending_) {
		return false;
	}

	isSending_ = true;

	WSABUF buf[2] = {};
	DWORD sendbytes = 0;
	buf[0].buf = sendQ_.GetFrontBufferPtr();
	buf[0].len = sendQ_.DirectDequeueSize();
	buf[1].buf = sendQ_.GetBufferPtr();
	buf[1].len = sendQ_.GetUseSize() - buf[0].len;

	this->IncrementRefCount();
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
		this->DecrementRefCountAndReleaseLock();
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

				CPacket* pkt = new CPacket(); // FIX
				recvQ_.Dequeue(pkt->GetBufferPtr(), header.Len);
				pkt->MoveWritePos(header.Len);
				OnMessage(0, pkt);
			}
			else break;
		}
		else break;
	}
	this->DecrementRefCountLock();
	if (!this->IsActive())
		return;
	this->PostRecv();
}

void Session::OnSend(DWORD transferred)
{
	SLog(DEBUG_LEVEL, L"sended: %d bytes", transferred);
	std::unique_lock<std::mutex> lock(mutex_);
	this->DecrementRefCount();
	if (!this->IsActive())
		return;
	sendQ_.MoveFront(transferred);
	isSending_ = false;
	if (sendQ_.GetUseSize() > 0) {
		lock.unlock();
		this->PostSend();
	}
}

void Session::OnMessage(uint16_t type, CPacket* packet) const
{
	PacketProcess_Auto::Instance().Process(type, id_, *packet);
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
		delete& packet; // FIX
	}
	this->PostSend();
}

void Session::IncrementRefCount()
{
	++refCount;
}


void Session::IncrementRefCountLock()
{
	std::lock_guard<std::mutex> lock(mutex_);
	this->IncrementRefCount();
}

void Session::DecrementRefCount()
{
	auto count = --refCount;
	if (disconnectRequested_ && count < 1) {
		SessionManager::Instance().DisconnectSession(id_);
	}
}

void Session::DecrementRefCountLock()
{
	std::lock_guard<std::mutex> lock(mutex_);
	this->DecrementRefCount();
}

void Session::DecrementRefCountAndRelease()
{
	auto count = --refCount;
	if (count < 1) {
		SessionManager::Instance().DisconnectSession(id_);
	}
	else {
		this->RequestDisconnect();
	}
}

void Session::DecrementRefCountAndReleaseLock()
{
	std::lock_guard<std::mutex> lock(mutex_);
	this->DecrementRefCountAndRelease();
}

bool Session::CheckHeader(PktHeader& header)
{
	// DOS
	return true;
}