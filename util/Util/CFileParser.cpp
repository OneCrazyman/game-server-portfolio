#include "CFileParser.h" 
#include <cstring>
#include <cstdlib>

CFileParser::~CFileParser()
{
	if (p_buffer_ != nullptr)
	{
		delete[] p_buffer_;
	}
}

bool CFileParser::Parse(const char* filePath)
{
	FILE* p_file;
	fopen_s(&p_file,filePath, "rb");
	if (p_file == nullptr) return false;

	long file_size = GetFileSize(p_file);
	buffer_size_ = file_size + 1;
	p_buffer_ = new char[buffer_size_];

	size_t ret = fread(p_buffer_, 1, file_size, p_file);
	if (ret != file_size)
	{
		fclose(p_file);
		delete[] p_buffer_;
		p_buffer_ = nullptr;
		return false;
	}
	p_buffer_[file_size] = '\0';
	fclose(p_file);
	EraseComments();
	return true;
}

bool CFileParser::SetSection(const char* section)
{
	char* curr = p_buffer_;
	char tmp_section[(int)MaxSize::MaxTextSize] = {};

	if (curr == nullptr)
		return false;

	for (; *curr != '\0'; ++curr)
	{
		if (*curr == '[')
		{
			++curr;
			int i = 0;
			for (; i < (int)MaxSize::MaxTextSize;)
			{
				if (*curr == ']')
				{
					break;
				}
				if (*curr == ' ')
				{
					++curr;
					continue;
				}
				tmp_section[i] = *curr;
				++curr;
				++i;
			}
			if (i >= (int)MaxSize::MaxTextSize - 1)
			{
				break;
			}
			tmp_section[i] = '\0';

			if (strcmp(section, tmp_section) == 0)
			{
				p_section_ = ++curr;
				return true;
			}
		}
	}
	return false;
}

bool CFileParser::GetIntValue(const char* key, int* value)
{
	char buffer[(int)MaxSize::MaxTextSize] = {};
	if (GetValueBuffer(key, buffer))
	{
		*value = atoi(buffer);
		return true;
	}
	return false;
}

bool CFileParser::GetStringValue(const char* key, char* value)
{
	return GetValueBuffer(key, value);
}

bool CFileParser::GetValueBuffer(const char* key, char* buffer)
{
	if (p_section_ == nullptr)
	{
		return false;
	}

	char* curr = p_section_;
	char tmp_buffer[(int)MaxSize::MaxTextSize] = {};
	bool in_quot = false;
	bool should_read_value = false;
	bool is_key_position = true;
	int scope_depth = 0;
	int tmp_count = 0;

	while (*curr != '\0')
	{
		switch (*curr)
		{
		case '\t':
		case '\n':
		case '\r':
		case ' ':
			++curr;
			continue;
		case '"':
			if (in_quot == true)
			{	// Key일때만 분기 실행
				if (is_key_position == true)
				{
					tmp_buffer[tmp_count] = '\0';
					if (strcmp(key, tmp_buffer) == 0)
					{
						should_read_value = true;
					}
					tmp_count = 0;
				}
				else if (should_read_value == false)
				{
					tmp_count = 0;
				}
			}
			in_quot = !in_quot;
			++curr;
			continue;
		case ',':
			is_key_position = true;
			if (should_read_value == true)
			{
				tmp_buffer[tmp_count++] = '\0';
				break;
			}
			++curr;
			continue;
		case ':':
			is_key_position = false;
			++curr;
			continue;
		case '{':
			++scope_depth;
			++curr;
			continue;
		case '}':
			--scope_depth;
			if (scope_depth == 0)
			{
				// Section end
				if (should_read_value)
				{
					tmp_buffer[tmp_count++] = '\0';
				}
				break;
			}
			++curr;
			continue;
		default:
			if (in_quot || should_read_value)
			{
				if (tmp_count >= (int)MaxSize::MaxTextSize - 1)
				{
					return false;
				}
				tmp_buffer[tmp_count++] = *curr;
			}
			++curr;
			continue;
		}
		// 맞는 키를 찾았을 때
		if (should_read_value == true)
		{
			strcpy_s(buffer, tmp_count, tmp_buffer);
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

long CFileParser::GetFileSize(FILE* filePtr)
{
	fseek(filePtr, 0, SEEK_END);
	long result = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);
	return result;
}

void CFileParser::EraseComments() const
{
	char* curr = p_buffer_;
	for (long i = 0; i < buffer_size_; i++)
	{
		if (*curr == '/' && *(curr + 1) == '/')
		{
			while (*curr != '\r' && *curr != '\n' && *curr != '\0')
			{
				*curr++ = ' ';
			}
		}
		++curr;
	}
}
