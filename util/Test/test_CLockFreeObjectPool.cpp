//#include <gtest/gtest.h>
//
//#include "CLockFreeObjectPool.h"
//
//using core::CLockFreeObjectPool;
//
//class Player
//{
//public:
//	static int alloc_count;
//	Player() { ++alloc_count; }
//	int x;
//	int y;
//	int hp;
//	int speed;
//};
//int Player::alloc_count;
//
//const int LOOP_COUNT = 10000;
//
//TEST(CLockFreeObjectPool, CheckIdTest)
//{
//	for (int i = 0;i < LOOP_COUNT;i++)
//	{
//		CLockFreeObjectPool<Player> tmpPlayerPool;
//		Player* p1;
//		p1 = tmpPlayerPool.Alloc();
//		unsigned int p1_id = *(unsigned int*)((PBYTE)p1 - sizeof(unsigned int));
//		EXPECT_EQ(p1_id, i+1);	// 같은 인스턴스일때 같은 값인지
//	}
//}
//
//TEST(CLockFreeObjectPool, CastingPointerTest)
//{
//	CLockFreeObjectPool<Player> player_free_list;
//	Player** pPlayer_array = new Player * [LOOP_COUNT];
//
//	Player* pPrev;
//	pPrev = player_free_list.Alloc();
//	player_free_list.Free(pPrev);
//	for (int i = 0; i < LOOP_COUNT; i++)
//	{
//		Player* pCurr = player_free_list.Alloc();
//		EXPECT_EQ(pCurr, pPrev);
//		player_free_list.Free(pCurr);
//		pPrev = pCurr;
//	}
//	delete[] pPlayer_array;
//}
//
//TEST(CLockFreeObjectPool, AllocFreeCountTest)
//{
//	CLockFreeObjectPool<Player> player_free_list;
//	Player** pPlayer_array = new Player * [LOOP_COUNT];
//	Player::alloc_count = 0;
//	for (int i = 0; i < LOOP_COUNT; i++)
//	{
//		Player* tmp;
//		tmp = player_free_list.Alloc();
//		pPlayer_array[i] = tmp;
//	}
//	for (int i = 0; i < LOOP_COUNT; i++)
//	{
//		player_free_list.Free(pPlayer_array[i]);
//	}
//	EXPECT_EQ(LOOP_COUNT, Player::alloc_count);
//	for (int i = 0; i < LOOP_COUNT; i++)
//	{
//		Player* tmp;
//		tmp = player_free_list.Alloc();
//		pPlayer_array[i] = tmp;
//	}
//	EXPECT_EQ(LOOP_COUNT, Player::alloc_count);
//	delete[] pPlayer_array;
//}
//
//TEST(CLockFreeObjectPool, MismatchIdTest)
//{
//	CLockFreeObjectPool<Player> my_player_free_list;
//	CLockFreeObjectPool<Player> other_player_free_list;
//	// Alloc받아 할당
//	for (int i = 0; i < LOOP_COUNT; i++)
//	{
//		Player* p1;
//		Player* p2;
//		p1 = my_player_free_list.Alloc();
//		p2 = other_player_free_list.Alloc();
//		// id 값 추출
//		unsigned int my_id = *(unsigned int*)((char*)p1 - 4);
//		unsigned int other1_id = *(unsigned int*)((char*)p2 - 4);
//		EXPECT_NE(my_id, other1_id);	// 아이디 카운트되는지 체크
//		my_player_free_list.Free(p1);
//		other_player_free_list.Free(p2);
//	}
//}
//
//class Monster
//{
//public:
//	Monster()
//	{
//		ctor++;
//	}
//	~Monster()
//	{
//		dtor++;
//	}
//	inline static int ctor = 0;
//	inline static int dtor = 0;
//};
//
//TEST(CLockFreeObjectPool, PlacementNewTest)
//{
//	CLockFreeObjectPool<Monster> monsterPool(0, false);
//	Monster* m1 = monsterPool.Alloc(); // insert one
//	monsterPool.Free(m1);
//	for (int i = 0; i < LOOP_COUNT; i++)
//	{
//		Monster* m1 = monsterPool.Alloc();
//		monsterPool.Free(m1);
//		EXPECT_EQ(m1->ctor, 1);
//		EXPECT_EQ(m1->dtor, 0);
//	}
//	CLockFreeObjectPool<Monster> monsterPoolPNew(0,true);
//	Monster::ctor = 0;
//	Monster::dtor = 0;
//	for (int i = 0; i < LOOP_COUNT; i++)
//	{
//		Monster* m1 = monsterPoolPNew.Alloc();
//		monsterPoolPNew.Free(m1);
//		EXPECT_EQ(m1->ctor, i + 1);
//		EXPECT_EQ(m1->dtor, i + 1);
//	}
//}
//
//TEST(CLockFreeObjectPool, DISABLED_LoopTest)
//{
//	const int allocFreeCount = 1000;
//	CLockFreeObjectPool<Player> player_free_list;
//	Player** pPlayer_array = new Player*[LOOP_COUNT];
//	Player::alloc_count = 0;
//	for (int i = 0; i < LOOP_COUNT; i++)
//	{
//		for (int i = 0; i < allocFreeCount; i++)
//		{
//			Player* tmp;
//			tmp = player_free_list.Alloc();
//			pPlayer_array[i] = tmp;
//		}
//		for (int i = 0; i < allocFreeCount; i++)
//		{
//			player_free_list.Free(pPlayer_array[i]);
//		}
//	}
//	EXPECT_EQ(Player::alloc_count, LOOP_COUNT);
//}