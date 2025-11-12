#include <gtest/gtest.h>
#include "Util/CPacket.h"

struct cData // 6byte
{
	char header;
	short len;
	short type;
};

TEST(CPacketTest, InitTest)
{
	CPacket cPacket;

	EXPECT_EQ(cPacket.GetBufferSize(), 1400);
	EXPECT_EQ(cPacket.GetDataSize(), 0);
}

TEST(CPacketTest, GetAndPutData)
{
	CPacket cPacket;

	cData cSrc{ (char)0x89,128,1 };
	cData cDest{};
	cPacket.PutData((char*)&cSrc, sizeof(cData));
	cPacket.GetData((char*)&cDest, sizeof(cData));
	EXPECT_EQ(memcmp((char*)&cSrc, (char*)&cDest, sizeof(cData)), 0);
}

TEST(CPacketTest, OperatorOverridingTest)
{
	CPacket cPacket;

	int hp = 100;
	int mp = 80;
	int exp = 500;

	cPacket << hp << mp << exp;

	int d_hp, d_mp, d_exp;
	cPacket >> d_hp >> d_mp >> d_exp;
	EXPECT_EQ(hp, d_hp);
	EXPECT_EQ(mp, d_mp);
	EXPECT_EQ(exp, d_exp);
}

TEST(CPacketTest, OperatorCheckInvalidType)
{
	__int64 i64 = 0;
	CPacket cPacket;
	cData c{};
	//cPacket << c; // is compiler err
	long long ll64 = 0;
	cPacket << ll64;
}

TEST(CPacketTest, ErrorCheck)
{
	CPacket cPacket(16);
	cPacket << 1;
	cPacket << 2;
	cPacket << 3;
	cPacket << 4;
	EXPECT_EQ(cPacket.GetLastError(), CPacket::en_ERROR::NONE);
	cPacket << 5;
	EXPECT_EQ(cPacket.GetLastError(), CPacket::en_ERROR::BUFFER_FULL);
	EXPECT_EQ(cPacket.GetDataSize(), 16);
	CPacket copy_cPacket(16);

	copy_cPacket = cPacket; // copy assignmet
	CPacket copycon_cPacket = cPacket;	// copy constructor

	// origin
	int result[5] = {};
	for (int i = 0; i < 5; i++)
	{
		cPacket >> result[i];
	}
	EXPECT_EQ(cPacket.GetLastError(), CPacket::en_ERROR::BUFFER_EMPTY);
	for (int i = 0; i < 4; i++)
	{
		EXPECT_EQ(result[i], i + 1);
	}
	// copy assign
	int copy_result[5] = {};
	for (int i = 0; i < 5; i++)
	{
		copy_cPacket >> copy_result[i];
	}
	EXPECT_EQ(copy_cPacket.GetLastError(), CPacket::en_ERROR::BUFFER_EMPTY);
	for (int i = 0; i < 4; i++)
	{
		EXPECT_EQ(copy_result[i], i + 1);
	}
	// copy constructor
	int copycon_result[5] = {};
	for (int i = 0; i < 5; i++)
	{
		copycon_cPacket >> copycon_result[i];
	}
	EXPECT_EQ(copycon_cPacket.GetLastError(), CPacket::en_ERROR::BUFFER_EMPTY);
	for (int i = 0; i < 4; i++)
	{
		EXPECT_EQ(copycon_result[i], i + 1);
	}
}

TEST(CPacketTest, PutData_BoundaryTest)
{
	CPacket pkt(10);
	
	pkt << (int)1 << (int)2;
	int a, b;
	pkt >> a >> b;
	pkt << (int)3;
	EXPECT_EQ(pkt.GetLastError(), CPacket::en_ERROR::BUFFER_FULL);
}