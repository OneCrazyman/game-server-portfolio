#include "stdafx.h"
#include "SessionManager.h"
#include "Util/SystemLogger.h"
#include "define.h"
#include "NetworkEvents.h"

using enum SystemLogger::LOG_LEVEL;

std::shared_ptr<Session> SessionManager::CreateSession(SOCKET clientSocket, SOCKADDR_IN& clientaddr, SessionInfo& sessionInfo)
{
	PrepareAsyncSendBuffer(clientSocket);
	SetLingerOptionRST(clientSocket);

	uint32_t sessionId;
	if (GetIdLock(sessionId) == false) 
		return nullptr;

    sessions_[sessionId].get()->Init(clientSocket, sessionId, clientaddr, sessionInfo);
	return sessions_[sessionId];
}

void SessionManager::Init(NetworkEvents* networkEvents, uint32_t maxSession, uint32_t maxRecvQ, uint32_t maxSendQ)
{
	networkEvents_ = networkEvents;
	maxSession_ = maxSession;
	SessionsInit(maxRecvQ, maxSendQ);
	InitSessionIdQueue();
}

void SessionManager::DisconnectSession(uint32_t id)
{
	auto session = sessions_[id];
	session->Clear();
	this->FreeIdLock(id);
	networkEvents_->OnRelease(id);
	SLog(DEBUG_LEVEL, L"[TCP server] client disconnect: ip = %ls,port = %d\n",session->Ip(),session->Port());
}

std::shared_ptr<Session> SessionManager::GetSessionPointer(uint32_t id)
{
	auto session = sessions_[id];
	if (session->IsActiveLock() == false)
		return nullptr;
	return session;
}

void SessionManager::PrepareAsyncSendBuffer(SOCKET clientSocket)
{
	int sndbufsize = 0;
	int ret = setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sndbufsize, sizeof(sndbufsize));
	if (ret == SOCKET_ERROR) {
		int error = WSAGetLastError();
		SLog(ERROR_LEVEL, L"(%d)", error);
	}
}

void SessionManager::SetLingerOptionRST(SOCKET clientSocket)
{
	linger opt;
	opt.l_linger = 1;
	opt.l_onoff = 0;
	setsockopt(clientSocket, SOL_SOCKET, SO_LINGER, (char*)&opt, sizeof(opt));
}

void SessionManager::InitSessionIdQueue()
{
	for (uint32_t i = 0; i < maxSession_; i++)
	{
		sessionIdStack_.push(i);
	}
}

void SessionManager::SessionsInit(uint32_t maxRecvQ, uint32_t maxSendQ)
{
	sessions_.reserve(maxSession_);
	for (uint32_t i = 0; i < maxSession_; i++) {
		sessions_.emplace_back(std::make_shared<Session>(maxRecvQ, maxSendQ));
	}
}

bool SessionManager::GetIdLock(uint32_t& id)
{
	std::lock_guard<std::mutex> lock(sessionIdQueueMutex_);
	if (!sessionIdStack_.empty()) {
		id = sessionIdStack_.top();
		sessionIdStack_.pop();
		return true;
	}
	else {
		SLog(DEBUG_LEVEL, L"sessionIdStack_ is empty");
		return false;
	}
}

void SessionManager::FreeIdLock(uint32_t id)
{
	std::lock_guard<std::mutex> lock(sessionIdQueueMutex_);
	sessionIdStack_.push(id);
}