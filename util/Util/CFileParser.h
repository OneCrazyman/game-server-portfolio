#pragma once
#include <climits>
#include <cstdio>

class CFileParser
{
	enum class MaxSize : unsigned char
	{
		MaxTextSize = UCHAR_MAX,
	};

public:
	CFileParser() = default;
	~CFileParser();

	bool Parse(const char* filePath);
	bool SetSection(const char* section);
	bool GetIntValue(const char* key, int* value);
	bool GetStringValue(const char* key, char* value);

private:
	bool GetValueBuffer(const char* key, char* buffer);
	long GetFileSize(FILE* filePtr);
	void EraseComments() const;

private:
	char* p_buffer_ = nullptr;
	char* p_section_ = nullptr;
	long buffer_size_ = 0;
};

