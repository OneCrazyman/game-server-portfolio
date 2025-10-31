#include "stdafx.h"
#include "Server.h"
#include "NetServer.h"
#include "ConsoleHandler.h"
#include "Lobby.h"
#include "SystemLogger.h"

int main()
{
	SystemLogger::Instance().SYSLOG_LEVEL(SystemLogger::LOG_LEVEL::ERROR_LEVEL);

	NetworkEvents* lobby = &Lobby::Instance();

	std::shared_ptr<Server> server = std::make_shared<NetServer>(lobby, 4,8);

	server.get()->Start();

	ConsoleHandler console(server);
	console.Run();

	return 0;
}