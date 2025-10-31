#include "stdafx.h"
#include "PacketMaker_Auto.h"
#include "define.h"

void mp_Echo(CPacket& packet, int64_t msg)
{
	PktHeader pktHeader;
	//pktHeader.Code = 0x98; 
	//pktHeader.Type = 0;
	pktHeader.Len = sizeof(int64_t);
	packet.Clear();
	packet.PutData((char*)&pktHeader, sizeof(PktHeader));
	packet << msg;
}