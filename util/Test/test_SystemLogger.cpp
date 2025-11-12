#include <gtest/gtest.h>
#include "Util/SystemLogger.h"
#include <thread>

using enum SystemLogger::LOG_LEVEL;

#define LOG (SystemLogger::Instance())

class SystemLogger_Test : public testing::Test
{
public:
    void ThreadOne();
    void ThreadTwo();
};

void SystemLogger_Test::ThreadOne()
{
    for (int i = 0; i < 10'000; i++) {
        SLog(SYSTEM_LEVEL,L"thread one func ::: hello");
    }
}

void SystemLogger_Test::ThreadTwo()
{
    for (int i = 0; i < 10'000; i++) {
        SLog(SYSTEM_LEVEL, L"thread two func ::: world");
    }
}
