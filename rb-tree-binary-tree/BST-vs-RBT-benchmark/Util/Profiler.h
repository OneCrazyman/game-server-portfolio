// thread safety profiler used TLS

#pragma once
#include <iostream>
#include <chrono>
#include <vector>
#include <mutex>

#include "Util/SystemLogger.h"
#include "Util/Util.h"

using enum SystemLogger::LOG_LEVEL;

#define CONCAT_IMPL(a, b) a ## b
#define CONCAT(a, b) CONCAT_IMPL(a, b)

#ifdef ENABLED_PROFILER
#define CREATE_PROFILE \
    Profiler CONCAT(profiler, __LINE__)(__func__)

#define CREATE_PROFILE_TAG(tag) \
    Profiler CONCAT(profiler, __LINE__)(tag)
#else
#define CREATE_PROFILE 
#define CREATE_PROFILE_TAG(tag)
#endif

#define PROFILE_MAX 128

struct PROFILE_DATA
{
	bool Flag;						// 프로파일의 사용 여부. (배열시에만)
	char Name[64];					// 프로파일 샘플 이름.

	__int64	TotalTime;				// 전체 사용시간 카운터 Time.	(출력시 호출회수로 나누어 평균 구함)
	__int64	Min[2];					// 최소 사용시간 카운터 Time.	(나노초단위로 계산하여 저장 / [0] 가장최소 [1] 다음최소)
	__int64	Max[2];					// 최대 사용시간 카운터 Time.	(나노초단위로 계산하여 저장 / [0] 가장최대 [1] 다음최대)

	__int64	Call;					// 누적 호출 횟수.
};

struct PROFILE_THREAD_CONTEXT
{
	std::unique_ptr<PROFILE_DATA[]> ProfileData;
	std::string ThreadId;
	int ProfileCount;
};

class Profiler
{
	using Clock = std::chrono::steady_clock; // wraps QueryPerformanceCounter
	using TimePoint = Clock::time_point;

public:
	Profiler(const char* tag);
	~Profiler();

	static bool DataOutText(const char* szFileName);

private:
	void CheckInit();
	void Initialize();
	void Begin(const char* tag);
	void End();						
	void Update(int count);

private:
	inline static std::mutex mtx;
	inline static std::vector<std::unique_ptr<PROFILE_THREAD_CONTEXT>> allProfiles;
	inline static thread_local PROFILE_THREAD_CONTEXT* tlsContext = nullptr;
	inline static thread_local bool isInit = false;

private:
	TimePoint startTime;
	TimePoint endTime;

	const char* tag_;
};

inline Profiler::Profiler(const char* tag) : tag_(tag)
{
	this->CheckInit();
	this->Begin(tag);
}

inline Profiler::~Profiler()
{
	this->End();
}

inline void Profiler::Initialize()
{
	for (size_t i = 0; i < PROFILE_MAX; i++) {
		tlsContext->ProfileData[i].Call = 0;
		tlsContext->ProfileData[i].Flag = false;
		tlsContext->ProfileData[i].Max[0] = 0;
		tlsContext->ProfileData[i].Max[1] = 0;
		tlsContext->ProfileData[i].Min[0] = LLONG_MAX;
		tlsContext->ProfileData[i].Min[1] = LLONG_MAX;
		tlsContext->ProfileData[i].Name[0] = '\0';
		tlsContext->ProfileData[i].TotalTime = 0;
	}
}

inline void Profiler::CheckInit()
{
	if (isInit) return;

	isInit = true;
	
	auto ctx = std::make_unique<PROFILE_THREAD_CONTEXT>();
	ctx->ThreadId = util::GetThreadId(std::this_thread::get_id());
	ctx->ProfileData = std::make_unique<PROFILE_DATA[]>(PROFILE_MAX);
	ctx->ProfileCount = 0;

	tlsContext = ctx.get();
	this->Initialize();

	std::lock_guard<std::mutex> lock(mtx);
	allProfiles.push_back(std::move(ctx));
}

inline static double GetAvgUs(PROFILE_DATA& profileData)
{
	const double NS_TO_US = 1.0 / 1000.0;
	const int MIN_MAX_SUM = 4; 
	if (profileData.Call <= MIN_MAX_SUM) {
		if (profileData.Call == 0) return 0.0;
		return (profileData.TotalTime) * NS_TO_US / static_cast<double>(profileData.Call);
	}
	else {
		__int64 sumMinMaxNs =
			// 양 극단값 2개씩(총4) 제외 
			profileData.Min[0] +
			profileData.Min[1] +
			profileData.Max[0] +
			profileData.Max[1];
		return (profileData.TotalTime - sumMinMaxNs) * NS_TO_US / static_cast<double>(profileData.Call - MIN_MAX_SUM);
	}
}

inline static void WriteProfileData(FILE* fp, PROFILE_THREAD_CONTEXT* ctx)
{
	const double NS_TO_US = 1.0 / 1000.0;

	PROFILE_DATA* profileData = ctx->ProfileData.get();
	const char* threadId = ctx->ThreadId.c_str();
	int count = ctx->ProfileCount;

	fprintf_s(fp, "Thread \t| Name \t\t| Average \t| Min \t\t| Max \t\t| Call \t|\n");
	for (int i = 0; i < count; i++) {
		double avgUs = GetAvgUs(profileData[i]);
		fprintf_s(fp, "%s \t| %s \t| %.4lf㎲ \t| %.4lf㎲ \t| %.4lf㎲ \t| %lld \t|\n",
			threadId,
			profileData[i].Name,
			avgUs,
			profileData[i].Min[0] * NS_TO_US,
			profileData[i].Max[0] * NS_TO_US,
			profileData[i].Call
		);
	}

	char textLine[112];
	memset(textLine, '-', _countof(textLine));
	textLine[_countof(textLine) - 1] = '\0';
	fprintf_s(fp, "%s\n", textLine);
}

inline bool Profiler::DataOutText(const char* szFileName)
{
	FILE* fp;
	fopen_s(&fp, szFileName, "w");
	if (fp == nullptr)
	{
		SLog(ERROR_LEVEL, L"don't read file: %s\n", szFileName);
		return false;
	}
	{
		std::lock_guard<std::mutex> lock(mtx);
		for (int i = 0; i < allProfiles.size(); i++) {
			WriteProfileData(fp, allProfiles[i].get());
		}
	}
	fclose(fp);

	return true;
}


inline void Profiler::Begin(const char* tag)
{
	startTime = Clock::now();
}

inline void Profiler::End()
{
	endTime = Clock::now();
	
	bool isFind = false;
	int& count = tlsContext->ProfileCount;
	const auto pfData = tlsContext->ProfileData.get();

	for (int i = 0; i < count; i++)
	{
		if (strcmp(pfData[i].Name, tag_) == 0)
		{
			this->Update(i);
			isFind = true;
		}
	}

	if (!isFind)
	{
		this->Update(count);
		count++;
	}
}

// Profiler 구조체 배열에 데이터 업데이트 및 생성
inline void Profiler::Update(int count)
{
	if (count >= PROFILE_MAX)
	{
		SLog(ERROR_LEVEL, L"Profile count overflow # count: %d", count);
		return;
	}

	auto pfData = tlsContext->ProfileData.get();
	if (!pfData[count].Flag)
		strcpy_s(pfData[count].Name, strlen(tag_) + 1, tag_);

	auto elapsedNsTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();

	pfData[count].Flag = true;
	pfData[count].TotalTime += elapsedNsTime;
	pfData[count].Call++;

	// min 
	auto& mn0 = pfData[count].Min[0];
	auto& mn1 = pfData[count].Min[1];
	if (elapsedNsTime < mn0) {
		mn1 = mn0;
		mn0 = elapsedNsTime;
	}
	else if (elapsedNsTime < mn1) {
		mn1 = elapsedNsTime;
	}

	// max
	auto& mx0 = pfData[count].Max[0];
	auto& mx1 = pfData[count].Max[1];
	if (elapsedNsTime > mx0) {
		mx1 = mx0;
		mx0 = elapsedNsTime;
	}
	else if (elapsedNsTime > mx1) {
		mx1 = elapsedNsTime;
	}
}

