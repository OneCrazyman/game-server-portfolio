#include "stdafx.h"  
#include "SystemLogger.h"  

void SystemLogger::Write(LOG_LEVEL logLevel, const wchar_t* func, const wchar_t* fmt, ...)
{
    if (logLevel >= sysLogLevel_) {
        va_list args;
        va_start(args, fmt);
        wchar_t buf[1024];
        _vsnwprintf_s(buf, _countof(buf), _TRUNCATE, fmt, args);
        va_end(args);
        wprintf_s(L"[%ls] %-35ls: %ls\n", LevelToChar(logLevel), func, buf);
        // TODO FILE LOG
    }
}

void SystemLogger::SYSLOG_DIRECTORY(wchar_t*)
{
    // TODO 
}

void SystemLogger::SYSLOG_LEVEL(LOG_LEVEL loglevel)
{
    sysLogLevel_ = loglevel;
}

inline const wchar_t* SystemLogger::LevelToChar(LOG_LEVEL logLevel) const
{
    switch (logLevel) {
    case SystemLogger::LOG_LEVEL::DEBUG_LEVEL:
        return L"DEBUG";
    case SystemLogger::LOG_LEVEL::ERROR_LEVEL:
        return L"ERROR";
    case SystemLogger::LOG_LEVEL::SYSTEM_LEVEL:
        return L"SYSTEM";
    default:
        return L"UNKNOWN";
    }
}