#include "stdafx.h"
#include "ConsoleHandler.h"
#include "Util/SystemLogger.h"
#include "Util/Profiler.h"

using enum SystemLogger::LOG_LEVEL;

void ConsoleHandler::Run()
{
	while (1) {
		switch (_getch()) {
			case 's': {
				server_.get()->Stop();
				SLog(SYSTEM_LEVEL, L"## Server Stopped");
				continue;
			}
			case 'd': {
				SystemLogger::Instance().SYSLOG_LEVEL(DEBUG_LEVEL);
				continue;
			}
			case 'e': {
				SystemLogger::Instance().SYSLOG_LEVEL(ERROR_LEVEL);
				continue;
			}
			case 'p': {
				Profiler::DataOutText("profile.txt");
				continue;
			}
			default: {
				SLog(SYSTEM_LEVEL, L"UNKNOWN COMMAND");
				continue;
			}
		}
	}
	SLog(SYSTEM_LEVEL, L"Console Shutdown..");
	return;
}