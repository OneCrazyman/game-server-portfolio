#pragma once
#include "stdafx.h"
#include "Singleton.h"
#define SLog(level, fmt, ...) \
    SystemLogger::Instance().Write(level,__FUNCTIONW__, fmt, ##__VA_ARGS__)

class SystemLogger : public Singleton<SystemLogger>
{
	friend class Singleton<SystemLogger>;
public:
	enum class LOG_LEVEL
	{
		DEBUG_LEVEL,
		ERROR_LEVEL,
		SYSTEM_LEVEL,
	};

	// 파일 쓰기 구현 예정 우선 콘솔 출력 기능만 존재
	void Write(LOG_LEVEL logLevel, const wchar_t* func, const wchar_t* fmt, ...);

	void SYSLOG_DIRECTORY(wchar_t*);
	void SYSLOG_LEVEL(LOG_LEVEL);

private:
	SystemLogger() = default;

	inline const wchar_t* LevelToChar(LOG_LEVEL) const;

private:
	LOG_LEVEL sysLogLevel_ = LOG_LEVEL::ERROR_LEVEL;
};