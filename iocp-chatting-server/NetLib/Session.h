#pragma once
#include "stdafx.h"
#include "Util/CRingBuffer.h"
#include "define.h"

struct SessionConfig;
class CPacket;

class Session
{
public:
	enum class IO_TYPE
	{
		IO_RECV,
		IO_SEND,
	};

	enum class STATE
	{
		INACTIVE,
		ACTIVE,
	};

	struct MyOverlapped
	{
		WSAOVERLAPPED overlapped;
		IO_TYPE type;
	};

	Session(uint32_t maxRecvQ, uint32_t maxSendQ) : recvQ_(maxRecvQ), sendQ_(maxSendQ) {}

	void Init(SOCKET clientSocket, uint32_t id, SOCKADDR_IN& clientaddr, SessionConfig& sessionInfo);

	void ClearLock();
	void Clear();

	bool PostRecv();
	bool PostSend();

	void OnRecv(DWORD transferred);
	void OnSend(DWORD transferred);

	void SendPacket(CPacket&);

	bool IsActive() const { 
		return (state_ == STATE::ACTIVE ? true : false);
	}
	bool IsActiveLock() { std::lock_guard<std::mutex> lock(mutex_); return (state_ == STATE::ACTIVE ? true : false); }
	uint32_t UserId() const { return userId_; }
	uint32_t GetUserIdLock() { std::lock_guard<std::mutex> lock(mutex_); return userId_; }
	void SetUserId(uint32_t userId) { userId_ = userId; }
	void SetUserIdLock(uint32_t userId) { std::lock_guard<std::mutex> lock(mutex_); userId_ = userId; }
	uint32_t Id() const { return id_; }
	const wchar_t* Ip() { return ip_.c_str(); }
	uint16_t Port() const { return port_; }

	void RequestDisconnect() { disconnectRequested_ = true; }
	void IncrementRefCount();
	bool DecrementRefCount();
	void DecrementRefCountAndRelease();

	void CloseSocket() const { closesocket(socket_); }

	std::mutex& Mutex() { return mutex_; }
protected:
	void OnMessage(uint16_t type, CPacket& packet) const;

	bool CheckHeader(PktHeader&);

protected:
	uint32_t id_ = 0;
	uint32_t gen_ = 0;
	//std::atomic<STATE> state_ = STATE::INACTIVE;
	//std::atomic<long> refCount = 0;
	STATE state_ = STATE::INACTIVE;
	long refCount = 0;
	SOCKET socket_ = NULL;
	std::wstring ip_;
	uint16_t port_ = 0;

	CRingBuffer recvQ_;
	CRingBuffer sendQ_;

	MyOverlapped overlappedRecv_ = {};
	MyOverlapped overlappedSend_ = {};
	
	uint32_t genCount_ = 0;
	bool disconnectRequested_ = false;
	
	bool isSending_ = false;

	std::mutex mutex_;

	uint32_t userId_ = 0;
};

