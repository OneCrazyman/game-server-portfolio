#include <gtest/gtest.h>
#include "Util/ObjectPool.h"
#include <malloc.h>

class Player
{
public:
	static int alloc_count;
	Player() { ++alloc_count; }
	int x = 0;
	int y = 0;
	int hp = 0;
	int speed = 0;
};
int Player::alloc_count;

const int loop_count = 10'000;

TEST(ObjectPool, CastingPointerTest)
{
	ObjectPool<Player> player_pool;
	Player** pPlayer_array = new Player * [loop_count];

	Player* pPrev;
	pPrev = player_pool.Alloc();
	player_pool.Free(pPrev);
	for (int i = 0; i < loop_count; i++) {
		Player* pCurr = player_pool.Alloc();
		EXPECT_EQ(pCurr, pPrev);
		player_pool.Free(pCurr);
		pPrev = pCurr;
	}
	delete[] pPlayer_array;
}

TEST(ObjectPool, DISABLED_AllocFreeTest)
{
	ObjectPool<Player> player_pool;
	Player** pPlayer_array = new Player * [loop_count];
	Player::alloc_count = 0;
	for (int i = 0; i < loop_count; i++) {
		Player* tmp;
		tmp = player_pool.Alloc();
		pPlayer_array[i] = tmp;
	}
	for (int i = 0; i < loop_count; i++) {
		player_pool.Free(pPlayer_array[i]);
	}
	EXPECT_EQ(loop_count, Player::alloc_count);
	for (int i = 0; i < loop_count; i++) {
		Player* tmp;
		tmp = player_pool.Alloc();
		pPlayer_array[i] = tmp;
	}
	EXPECT_EQ(loop_count, Player::alloc_count);
	delete[] pPlayer_array;
}

TEST(ObjectPool, ReinterpretCastingTest)
{
	struct Node
	{
		Player Data;
		Node* Next;
	};
	Node next;
	Node* node = new Node{};
	node->Data.x = 0xff;
	node->Data.speed = 0xff;
	node->Next = &next;

	Player* pPlayer = reinterpret_cast<Player*>(&node->Data);
	EXPECT_EQ(pPlayer->x, 0xff);
	Node* cast_node = reinterpret_cast<Node*>(pPlayer);
	EXPECT_EQ(cast_node->Next, &next);
}

TEST(ObjectPool, LoopTest)
{
	ObjectPool<Player> player_pool;
	Player** pPlayer_array = new Player * [loop_count];
	Player::alloc_count = 0;
	for (int i = 0; i < loop_count; i++) {
		Player* tmp;
		tmp = player_pool.Alloc();
		pPlayer_array[i] = tmp;

	}
	for (int i = 0; i < loop_count; i++) {
		player_pool.Free(pPlayer_array[i]);
	}
	EXPECT_EQ(Player::alloc_count, loop_count);
	for (int i = 0; i < loop_count; i++) {
		Player* tmp;
		tmp = player_pool.Alloc();
		pPlayer_array[i] = tmp;
	}
	EXPECT_EQ(Player::alloc_count, loop_count);
}

TEST(ObjectPool, DISABLED_DestructorTest)
{
	ObjectPool<Player> player_pool;
	Player* pPlayer_array[loop_count];
	// Alloc받아 할당
	for (int i = 0; i < loop_count; i++) {
		Player* tmp;
		tmp = player_pool.Alloc();
		pPlayer_array[i] = tmp;
	}
	// 반환 
	for (int i = 0; i < loop_count / 2; i++) {
		player_pool.Free(pPlayer_array[i]);
	}
	// 어떻게 확인??
}

TEST(ObjectPool, CheckIdTest)
{
	ObjectPool<Player> my_player_pool;
	ObjectPool<Player> other_player_pool;
	// Alloc받아 할당
	for (int i = 0; i < loop_count; i++) {
		Player* p1;
		Player* p2;
		Player* p3;
		p1 = my_player_pool.Alloc();
		p2 = other_player_pool.Alloc();
		p3 = other_player_pool.Alloc();
		// id 값 추출
		unsigned int my_id = *(unsigned int*)((char*)p1 - 4);
		unsigned int other1_id = *(unsigned int*)((char*)p2 - 4);
		unsigned int other2_id = *(unsigned int*)((char*)p3 - 4);
		EXPECT_NE(my_id, other1_id);	// 아이디 카운트되는지 체크
		EXPECT_EQ(other1_id, other2_id);	// 같은 인스턴스일때 같은 값인지
		my_player_pool.Free(p1);
		other_player_pool.Free(p2);
		other_player_pool.Free(p3);
	}
}

TEST(ObjectPool, MismatchIdTest)
{
	ObjectPool<Player> my_player_pool;
	ObjectPool<Player> other_player_pool;
	// Alloc받아 할당
	for (int i = 0; i < loop_count; i++) {
		Player* p1;
		Player* p2;
		p1 = my_player_pool.Alloc();
		p2 = other_player_pool.Alloc();
		// id 값 추출
		unsigned int my_id = *(unsigned int*)((char*)p1 - 4);
		unsigned int other1_id = *(unsigned int*)((char*)p2 - 4);
		EXPECT_NE(my_id, other1_id);	// 아이디 카운트되는지 체크
		my_player_pool.Free(p1);
		other_player_pool.Free(p2);

	}
}

// 같은 포인터가 중복 Free됐을때 검증 로직 테스트
TEST(ObjectPool, DuplicationFreeTest)
{
	ObjectPool<Player> my_player_pool;
	//// Alloc받아 할당
	for (int i = 0; i < 1000; i++) {
		Player* p1;
		p1 = my_player_pool.Alloc();
		my_player_pool.Free(p1);
		EXPECT_THROW(my_player_pool.Free(p1), std::runtime_error); // err
	}
}

class Monster
{
public:
	Monster()
	{
		++ctor;
	}
	~Monster()
	{
		++dtor;
	}
	inline static int ctor = 0;
	inline static int dtor = 0;
};

TEST(ObjectPool, PlacementNewTest)
{
	ObjectPool<Monster> monster_free_list(0,true);
	for (int i = 0; i < loop_count; i++) {
		Monster* m1 = monster_free_list.Alloc();
		monster_free_list.Free(m1);
		EXPECT_EQ(m1->ctor, m1->dtor);
	}
}