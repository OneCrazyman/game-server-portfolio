#include <gtest/gtest.h>
#define ENABLED_PROFILER
#include "Util/Profiler.h"

void FuncOne()
{
	CREATE_PROFILE;
	int a = 0;
	a++;
}

void FuncTwo()
{
	CREATE_PROFILE_TAG("TagTest");
	int a = 0;
	for (int i = 0; i < 100; i++) {
		a++;
	}
}

TEST(ProfilerTest, DefaultTest)
{
	const int LOOP = 1000;
	for (int i = 0; i < LOOP; i++) {
		FuncOne();
		FuncTwo();
	}
	bool isSuccess = Profiler::DataOutText("profile.txt");
	EXPECT_TRUE(isSuccess);
}

void ThreadFunc()
{
	int a = 0;
	while (a < 10000) {
		CREATE_PROFILE;
		for (int i = 0; i < 100; i++) {
			a++;
			a--;
		}
		a++;
	}
}

TEST(ProfilerTest, MultiThreadTest)
{
	std::thread t1(ThreadFunc);
	std::thread t2(ThreadFunc);
	t1.join();

	bool isSuccess = Profiler::DataOutText("profile.txt");
	EXPECT_TRUE(isSuccess);
}

TEST(ProfilerTest, MacroTest)
{
	CREATE_PROFILE;
	CREATE_PROFILE;
}