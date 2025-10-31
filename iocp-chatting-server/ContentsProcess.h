#pragma once
#include "PacketHandler_Auto.h"
#include "Singleton.h"

class ContentsProcess : public Singleton<ContentsProcess> ,public PacketHandler_Auto
{
	friend class Singleton<ContentsProcess>;
public:
	// PacketHandler_Auto을(를) 통해 상속됨
	void OnEcho(uint32_t sessionId, int64_t msg) override;

private:
	ContentsProcess() = default;

	// PacketHandler_Auto을(를) 통해 상속됨
};