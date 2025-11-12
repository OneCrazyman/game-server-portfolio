#pragma once
#include <iostream>
#include <windows.h>

#define PROFILE_MAX 128

struct PROFILE_DATA
{
	long			lFlag;				// 프로파일의 사용 여부. (배열시에만)
	CHAR			szName[64];			// 프로파일 샘플 이름.

	LARGE_INTEGER	lStartTime;			// 프로파일 샘플 실행 시간.

	__int64			iTotalTime;			// 전체 사용시간 카운터 Time.	(출력시 호출회수로 나누어 평균 구함)
	__int64			iMin[2];			// 최소 사용시간 카운터 Time.	(초단위로 계산하여 저장 / [0] 가장최소 [1] 다음 최소 [2])
	__int64			iMax[2];			// 최대 사용시간 카운터 Time.	(초단위로 계산하여 저장 / [0] 가장최대 [1] 다음 최대 [2])

	__int64			iCall;				// 누적 호출 횟수.
};

class Profile
{
public:
	Profile(const char* function_name);
	~Profile();

	static void DataOutText(const char* szFileName);
	static void Initialize();
private:
	void Begin(const char* tag);
	void End();
	void Update(int count);
public:
	inline static int _profile_count = 0;
	inline static LARGE_INTEGER _freq_time;
	inline static PROFILE_DATA profile_data[PROFILE_MAX];
private:
	LARGE_INTEGER _start_time;
	LARGE_INTEGER _end_time;

	const char* _tag;
};

inline Profile::Profile(const char* function_name)
{
	Begin(function_name);
	_tag = function_name;
}

inline Profile::~Profile()
{
	End();
}

inline void Profile::DataOutText(const char* szFileName)
{
	FILE* fp;
	fopen_s(&fp, szFileName, "w");
	if (fp == nullptr)
	{
		printf("파일을 열 수 없습니다: %s\n", szFileName);
		return;
	}

	char text_line[100];
	memset(text_line, '-', _countof(text_line));
	text_line[sizeof(text_line) - 1] = '\0';
	fprintf_s(fp, "%s\n", text_line);
	fprintf_s(fp, " Name \t| Average \t| Min \t\t| Max \t\t| Call \t|\n");
	fprintf_s(fp, "%s\n", text_line);
	for (int i = 0; i < _profile_count; i++)
	{
		__int64 sum_min_max =
			profile_data[i].iMin[0] +
			profile_data[i].iMin[1] +
			profile_data[i].iMax[0] +
			profile_data[i].iMax[1];
		double avg = (profile_data[i].iTotalTime - sum_min_max) * 1'000'000.0 / (_freq_time.QuadPart) / (profile_data[i].iCall - 4);
		fprintf_s(fp, " %s \t| %.4lf㎲ \t| %.4lf㎲ \t| %.4lf㎲ \t| %d \t|\n",
			profile_data[i].szName,
			avg,
			profile_data[i].iMin[0] * 1'000'000.0 / _freq_time.QuadPart,
			profile_data[i].iMax[0] * 1'000'000.0 / _freq_time.QuadPart,
			profile_data[i].iCall
		);
	}
	fprintf_s(fp, "%s\n", text_line);
	fclose(fp); // fixed, 32에러 발생했었음
}

// 초기화
inline void Profile::Initialize()
{
	QueryPerformanceFrequency(&Profile::_freq_time);
	for (size_t i = 0; i < PROFILE_MAX; i++)
	{
		profile_data[i].iMax[0] = 0;
		profile_data[i].iMax[1] = 0;
		profile_data[i].iMin[0] = LLONG_MAX;
		profile_data[i].iMin[1] = LLONG_MAX;
	}
}

inline void Profile::Begin(const char* tag)
{
	QueryPerformanceCounter(&_start_time);
}

inline void Profile::End()
{
	QueryPerformanceCounter(&_end_time);
	//
	bool is_find = false;
	for (size_t i = 0; i < _profile_count; i++)
	{
		// 구조체 배열에서 해당 태그를 찾음
		if (strcmp(profile_data[i].szName, _tag) == 0)
		{
			// 해당 태그의 구조체 배열에 데이터를 삽입
			Update(i);
			is_find = true;
		}
	}

	// 최초 생성시
	if (is_find == false)
	{
		Update(_profile_count);
		_profile_count++;
	}

	LONGLONG time = _end_time.QuadPart - _start_time.QuadPart;
	wprintf(L"time: %.4lf\n", time * 1'000'000.0 / _freq_time.QuadPart);
}

// Profile 구조체 배열에 데이터 갱신 및 생성
inline void Profile::Update(int count)
{
	if (count >= PROFILE_MAX)
	{
		// do
		return;
	}
	__int64 time = _end_time.QuadPart - _start_time.QuadPart;
	strcpy_s(profile_data[count].szName, strlen(_tag) + 1, _tag);
	profile_data[count].iTotalTime += time;
	// min 
	if (time < profile_data[count].iMin[0])
	{
		profile_data[count].iMin[0] = time;
	}
	else if (time < profile_data[count].iMin[1])
	{
		profile_data[count].iMin[1] = time;
	}
	// max
	if (time > profile_data[count].iMax[0])
	{
		profile_data[count].iMax[0] = time;
	}
	else if (time > profile_data[count].iMax[1])
	{
		profile_data[count].iMax[1] = time;
	}
	profile_data[count].iCall++;
}
