#include "stdafx.h"
#include "ConsoleHandler.h"
#include "Util/SystemLogger.h"

using enum SystemLogger::LOG_LEVEL;

void ConsoleHandler::Run()
{
	std::wstring command;
	while (1) {
		std::wcin >> command;
		if (command == L"stop") {
			server_.get()->Stop();
			SLog(SYSTEM_LEVEL, L"## Server Stopped");
			break;
		}
		if (command == L"debug" || command == L"d") {
			SystemLogger::Instance().SYSLOG_LEVEL(DEBUG_LEVEL);
		}
		if (command == L"error" || command == L"e") {
			SystemLogger::Instance().SYSLOG_LEVEL(ERROR_LEVEL);
		}
		else {
			SLog(SYSTEM_LEVEL, L"UNKNOWN COMMAND");
			continue;
		}
	}
	SLog(SYSTEM_LEVEL, L"Console Shutdown..");
	return;
}