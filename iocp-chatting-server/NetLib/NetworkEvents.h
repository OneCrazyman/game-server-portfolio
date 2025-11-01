#pragma once
#include "stdafx.h"

class NetworkEvents
{
public:
	virtual void OnAccept(uint32_t sessionId) = 0;
	virtual void OnRelease(uint32_t sessionId) = 0;
};