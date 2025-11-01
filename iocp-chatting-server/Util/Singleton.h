#pragma once
#include "stdafx.h"
template <class T>
class Singleton
{
protected:
	Singleton() = default;
	~Singleton() = default;

public:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	static T& Instance()
	{
		static T instance;
		return instance;
	}
};