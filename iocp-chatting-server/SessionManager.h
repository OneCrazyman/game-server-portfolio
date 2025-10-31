#pragma once
#include "stdafx.h"
#include "Session.h"
#include "Singleton.h"
#include "define.h"

class NetworkEvents;

class SessionManager : public Singleton<SessionManager>
{
	friend class Singleton<SessionManager>;

public:
	void Init(NetworkEvents* networkEvents,uint32_t maxSession, uint32_t maxRecvQ, uint32_t maxSendQ);

	std::shared_ptr<Session> CreateSession(SOCKET clientSocket, SOCKADDR_IN& clientaddr, SessionInfo& sessionInfo);

	void DisconnectSession(uint32_t id);

	std::shared_ptr<Session> GetSessionPointer(uint32_t id);
private:
	SessionManager() = default;

	void InitSessionIdQueue();

	void SessionsInit(uint32_t maxRecvQ, uint32_t maxSendQ);

	void PrepareAsyncSendBuffer(SOCKET);

	void SetLingerOptionRST(SOCKET);

	bool GetIdLock(uint32_t& id);

	void FreeIdLock(uint32_t id);

	std::vector<std::shared_ptr<Session>> sessions_;
	uint32_t maxSession_ = 0;
	
	std::stack<int> sessionIdStack_;
	std::mutex sessionIdQueueMutex_;

	NetworkEvents* networkEvents_ = nullptr;
};