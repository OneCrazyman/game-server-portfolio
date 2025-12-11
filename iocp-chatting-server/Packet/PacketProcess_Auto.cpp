#include "stdafx.h"
#include "PacketProcess_Auto.h"
#include "PacketHandler_Auto.h"

#include "Util/SystemLogger.h"
#include "Util/CPacket.h"
#include "NetLib/Session.h"
#include "Contents/ContentsProcess.h"

using enum SystemLogger::LOG_LEVEL;

void PacketProcess_Auto::Process(uint16_t type, uint32_t sessionId, CPacket& packet)
{
    switch (type) {
	case 0: DispatchEcho(sessionId, packet); break;
    default:
        SLog(ERROR_LEVEL, L"Packet Process default case # type is (%d)", type);
        break;
    }
}
void PacketProcess_Auto::DispatchEcho(uint32_t sessionId, CPacket& packet)
{
	int64_t msg;
	packet >> msg;
	handler.OnEcho(sessionId, msg);
}
