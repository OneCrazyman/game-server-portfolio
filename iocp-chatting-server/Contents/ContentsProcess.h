#pragma once
#include "Packet/PacketHandler_Auto.h"
#include "Util/Singleton.h"

class ContentsProcess : public Singleton<ContentsProcess> ,public PacketHandler_Auto
{
	friend class Singleton<ContentsProcess>;
public:
	void OnEcho(uint32_t sessionId, int64_t msg) override;

private:
	ContentsProcess() = default;

};