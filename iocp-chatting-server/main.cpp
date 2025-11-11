#include "stdafx.h"
#include "NetLib/Server.h"
#include "NetLib/NetServer.h"
#include "ConsoleHandler.h"
#include "Contents/Lobby.h"
#include "Util/SystemLogger.h"

int main()
{
	SystemLogger::Instance().SYSLOG_LEVEL(SystemLogger::LOG_LEVEL::DEBUG_LEVEL);

	NetworkEvents* lobby = &Lobby::Instance();

	std::shared_ptr<Server> server = std::make_shared<NetServer>(lobby, 1,1);

	server.get()->Start();

	ConsoleHandler console(server);
	console.Run();

	return 0;
}