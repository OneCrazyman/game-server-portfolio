#pragma once
#include "Singleton.h"
#include "PacketHandler_Auto.h"
#include "ContentsProcess.h"

class Session;
class CPacket;

class PacketProcess_Auto : public Singleton<PacketProcess_Auto>
{
    friend class Singleton<PacketProcess_Auto>;
public:
    void Process(uint16_t type, uint32_t sessionId, CPacket& packet);

private:
    PacketProcess_Auto() : handler(ContentsProcess::Instance()) {}
	void DispatchEcho(uint32_t sessionId, CPacket& packet);
    PacketHandler_Auto& handler;
};
