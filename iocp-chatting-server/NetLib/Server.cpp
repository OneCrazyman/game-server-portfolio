#include "stdafx.h"
#include "Server.h"

void Server::LoadConfig()
{
	// FIX : config파일 파싱으로 변경필요
	wcscpy_s(config_.Ip, _countof(ServerConfig::Ip), L"127.0.0.1");
	config_.MaxRecvQsize = 10000;
	config_.MaxSendQsize = 20000;
	config_.BackLogQsize = 256;
	config_.MaxSession = 15000;
	config_.Port = 6000;
	config_.PktHeaderCode = (char)0x89;
}