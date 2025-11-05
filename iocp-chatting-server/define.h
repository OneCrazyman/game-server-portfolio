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

enum WINSOCK_RET_CASE{
	SOCK_ERR_RST = 10054,
	SOCK_ERR_INTR = 10004,
	SOCK_ERR_NOTSOCK = 10038,
};