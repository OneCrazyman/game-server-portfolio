#include "stdafx.h"
#include "LanServer.h"
#include "Util/SystemLogger.h"
#include "SessionManager.h"
#include "Contents/lobby.h"
#include "define.h"

using enum SystemLogger::LOG_LEVEL;

bool LanServer::Start()
{
	SLog(SYSTEM_LEVEL, L"Server Start..");
	serverStopEvent_ = CreateEvent(NULL, TRUE, FALSE, NULL);

	this->LoadConfig();
	SessionManager::Instance().Init(networkEvents_,config_.MaxSession, config_.MaxRecvQsize, config_.MaxSendQsize);

	WSADATA wsa;
	auto retWsaStartUp = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (retWsaStartUp != 0) {
		SLog(ERROR_LEVEL, L"WSAStartup(%d)", retWsaStartUp);
		return false;
	}
	
	listenSocket_ = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket_ == INVALID_SOCKET) {
		int error = WSAGetLastError();
		SLog(ERROR_LEVEL, L"socket(%d)", error);
		return false;
	}

	SOCKADDR_IN serveraddr = {};
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(config_.Port);
	InetPton(AF_INET, config_.Ip, &serveraddr.sin_addr);

	int retBind = bind(listenSocket_, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retBind == SOCKET_ERROR) {
		int error = WSAGetLastError();
		SLog(ERROR_LEVEL, L"bind(%d)", error);
		return false;
	}

	int retListen = listen(listenSocket_, config_.BackLogQsize);
	if (retListen == SOCKET_ERROR) {
		int error = WSAGetLastError();
		SLog(ERROR_LEVEL, L"listen(%d)", error);
		return false;
	}
	SLog(SYSTEM_LEVEL, L"Listen..");

	completionPort_ = CreateNewCompletionPort(numberOfConcurrentThreads_);
	if (completionPort_ == INVALID_HANDLE_VALUE) {
		return false;
	}

	acceptThread_ = std::thread([this] {
		this->AcceptThread();
	});

	workerThreads_.reserve(numberOfWorkerThreads_);
	for (uint32_t i = 0;i < numberOfWorkerThreads_;i++) {
		workerThreads_.emplace_back(std::thread([this] {
			this->WorkerThread();
		}));
	}

	SLog(SYSTEM_LEVEL, L"## Server Start Success");
	return true;
}

LanServer::LanServer(NetworkEvents* networkEvents, DWORD numberOfConcurrentThreads, uint32_t numberOfWorkerThreads) :
	networkEvents_(networkEvents),
	numberOfConcurrentThreads_(numberOfConcurrentThreads),
	numberOfWorkerThreads_(numberOfWorkerThreads)
{}

void LanServer::Stop()
{
	SLog(SYSTEM_LEVEL, L"Server Stop..");

	CloseAcceptThread();
	CloseWorkerThreads();

	WaitingForMultiThreadEnd();
}

void LanServer::WorkerThread()
{
	while (1)
	{
		Session* session;
		Session::MyOverlapped* overlapped;
		DWORD cbTransferred;

		int retGQCS = GetQueuedCompletionStatus(completionPort_, &cbTransferred, (PULONG_PTR)&session,  (LPOVERLAPPED*)&overlapped, INFINITE);

		if (overlapped == 0 && cbTransferred == 0 && session == 0) {
			break;
		}

		if (cbTransferred == 0)
		{
			session->DecrementRefCountAndRelease();
			continue;
		}

		if (overlapped->type == Session::IO_TYPE::IO_RECV)
		{
			session->OnRecv(cbTransferred);
			continue;
		}
		else if (overlapped->type == Session::IO_TYPE::IO_SEND)
		{
			session->OnSend(cbTransferred);
			continue;
		}
	}

	SLog(SYSTEM_LEVEL, L"Shutdown WorkerThread ## id: %d", std::this_thread::get_id());
	return;
}

void LanServer::AcceptThread()
{
	while (1)
	{
		SOCKET clientSocket;
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		clientSocket = accept(listenSocket_, (SOCKADDR*)&clientaddr, &addrlen);
		if (clientSocket == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			switch (error) {
			case SOCK_ERR_RST:
				SLog(DEBUG_LEVEL, L"accept(%d)", error);
				continue;
			case SOCK_ERR_INTR:
			case SOCK_ERR_NOTSOCK:
				SLog(SYSTEM_LEVEL, L"Shutdown AcceptThread ## id: %d", std::this_thread::get_id());
				return;
			default:
				SLog(ERROR_LEVEL, L"accept(%d)",error);
				continue;
			}
		}
		OnAccept(clientSocket,clientaddr);
	}

	SLog(SYSTEM_LEVEL, L"Shutdown AcceptThread ## id: %d", std::this_thread::get_id());
	return;
}


HANDLE LanServer::CreateNewCompletionPort(DWORD dwNumberOfConcurrentThreads)
{
	HANDLE hnd = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);
	if (hnd == INVALID_HANDLE_VALUE) {
		DWORD lastError = GetLastError();
		SLog(ERROR_LEVEL, L"(%d)", lastError);
	}
	return hnd;
}

bool LanServer::AssociateDeviceWithCompletionPort(HANDLE hCompletionPort, HANDLE hDevice, ULONG_PTR dwCompletionKey)
{
	HANDLE hnd = CreateIoCompletionPort(hDevice, hCompletionPort, dwCompletionKey, 0);
	if (hnd == INVALID_HANDLE_VALUE) {
		DWORD lastError = GetLastError();
		SLog(ERROR_LEVEL, L"(%d)", lastError);
		return false;
	}
	return true;
}

void LanServer::WaitingForMultiThreadEnd()
{
	if (acceptThread_.joinable()) {
		acceptThread_.join();
	}

	for (auto& th : workerThreads_) {
		if (th.joinable()) {
			th.join();
		}
	}
}

void LanServer::CloseAcceptThread()
{
	closesocket(listenSocket_);
}

void LanServer::CloseWorkerThreads()
{
	for (uint32_t i = 0; i < numberOfWorkerThreads_; i++) {
		PostQueuedCompletionStatus(completionPort_, 0, 0, 0);
	}
}

void LanServer::OnAccept(SOCKET clientSocket, SOCKADDR_IN& clientaddr)
{
	SessionConfig sessionConfig = {
		config_.MaxRecvQsize,
		config_.MaxSendQsize,
	};

	auto session = sessionManager_.CreateSession(clientSocket, clientaddr, sessionConfig);
	if (!session) 
		return;

	bool associated = this->AssociateDeviceWithCompletionPort(completionPort_, (HANDLE)clientSocket, (ULONG_PTR)session.get());
	if (!associated) {
		sessionManager_.DisconnectSession(session->Id(),std::unique_lock<std::mutex>(session->Mutex()));	
		return;
	}

	bool recved = session->PostRecv();
	if (!recved) 
		return;

	uint32_t userId = networkEvents_->OnConnect(session->Id());
	session->SetUserId(userId);
}
