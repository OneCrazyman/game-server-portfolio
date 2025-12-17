#pragma once
#include <string>
#include <thread>

namespace util
{
	inline std::string GetThreadId(std::thread::id threadId)
	{
		std::stringstream ss;
		ss << threadId;
		return ss.str();
	}
}