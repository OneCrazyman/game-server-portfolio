#pragma once
#include "stdafx.h"
#include "Server.h"
#include "Util/CPacket.h"
#include "SessionManager.h"

class NetworkEvents;

class LanServer : public Server
{
public:
	LanServer(NetworkEvents*, DWORD numberOfConcurrentThreads, uint32_t numberOfWorkerThreads);
	bool Start();
	void Stop();

protected:
	void WorkerThread();
	void AcceptThread();

	HANDLE CreateNewCompletionPort(DWORD dwNumberOfConcurrentThreads);
	bool AssociateDeviceWithCompletionPort(HANDLE hCompletionPort, HANDLE hDevice, ULONG_PTR dwCompletionKey);
	void WaitingForMultiThreadEnd();
	void CloseAcceptThread();
	void CloseWorkerThreads();

protected:
	NetworkEvents* networkEvents_;
	DWORD numberOfConcurrentThreads_ = 0;
	uint32_t numberOfWorkerThreads_ = 0;

	HANDLE serverStopEvent_ = NULL;
	HANDLE completionPort_ = NULL;
	SOCKET listenSocket_ = INVALID_SOCKET;

	std::vector<std::thread> workerThreads_;
	std::thread acceptThread_;
	SessionManager& sessionManager_ = SessionManager::Instance();
};