#pragma once
struct PktHeader
{
	//short type;
	short Len;
};

struct SessionInfo
{
	int MaxRecvQsize;
	int MaxSendQsize;
};