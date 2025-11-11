#pragma once
struct SessionInfo;
class NetworkEvents
{
public:
	virtual uint32_t OnConnect(uint32_t sessionId) = 0;
	virtual void OnRelease(uint32_t sessionId, SessionInfo& sessionInfo) = 0;
};