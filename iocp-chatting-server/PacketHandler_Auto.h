#pragma once
class PacketHandler_Auto
{
public:
	virtual void OnEcho(uint32_t sessionId, int64_t msg) = 0;
};
