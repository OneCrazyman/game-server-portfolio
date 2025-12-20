#include "pch.h"
#include "RBtreeTest.h"

// 레드 블랙트리 검증 프로그램
// -----------
// 레드블랙트리 삽입, 삭제시 체크 포인트
// 1. root가 블랙인가
// 2. 모든 노드가 색을 가지고 있는가
// 3. 루트노드부터 리프노드까지 모든 경로의 블랙의 수는 같은가
// 4. 모든 리프노드는 Nil노드인가
// 5. 레드 노드의 자식이 블랙인가
// + 제대로 대소관계에 따라 정렬 되었는가 (기본 이진트리)
// -----------

// 기능 테스트
// 모든 기능에서 하나의 원소 Insert, Delete마다 RB로직을 재귀를 활용해 검증한다
TEST_F(RBtreeTest, InsertRandomNodes)
{
	GenerateRandomVector();
	InsertAll();
	SortVector(inserted_keys);
	SortDuplicate();
	std::vector<int> inorder_keys;
	tree.GetSortedKeyArray(inorder_keys);
	// 비교
	EXPECT_EQ(inserted_keys, inorder_keys);
}

TEST_F(RBtreeTest, DeleteSortedSequentialNodes)
{
	GenerateRandomVector();
	InsertAll();
	SortVector(inserted_keys);
	SortDuplicate();
	std::vector<int> inorder_keys;
	tree.GetSortedKeyArray(inorder_keys);
	// 비교
	EXPECT_EQ(inserted_keys, inorder_keys);
	// 제거
	DeleteAll();

	EXPECT_EQ(tree.GetRootNode(), tree.GetNilNodePtr());
	EXPECT_EQ(tree.GetCount(), 0);
}

TEST_F(RBtreeTest, DeleteSortedRandomShuffleNodes)
{
	GenerateRandomVector();
	InsertAll();
	SortVector(inserted_keys);
	SortDuplicate();
	std::vector<int> inorder_keys;
	tree.GetSortedKeyArray(inorder_keys);
	// 비교
	EXPECT_EQ(inserted_keys, inorder_keys);
	//// 제거
	// 원소를 셔플한 벡터를 기반으로 무작위 삭제한다
	ShuffleInsertedKeys();	
	DeleteAll();
	EXPECT_EQ(tree.GetRootNode(), tree.GetNilNodePtr());
	EXPECT_EQ(tree.GetCount(), 0);
}

// 스트레스 테스트
TEST_F(RBtreeTest, StressTreeTest)
{
	const int loop_count= 10'000;
	srand(0xFF);
	for (int i = 0; i < loop_count; i++)
	{
		int num_range = rand() % 1000;
		int vector_size = rand() % 100 + 100;
		GenerateRandomVector(vector_size, num_range);
		InsertAll();
		SortVector(inserted_keys);
		SortDuplicate();
		std::vector<int> inorder_keys;
		// 트리에서 중위 순회한 결과를 벡터에 삽입
		tree.GetSortedKeyArray(inorder_keys);
		// 값 비교 -> 제대로 대소관계에 따라 정렬 되었는가 (기본 이진트리 검증)
		EXPECT_EQ(inserted_keys, inorder_keys);
		// 원소를 셔플한 벡터를 기반으로 무작위 삭제한다
		ShuffleInsertedKeys();
		DeleteAll();
		EXPECT_EQ(tree.GetRootNode(), tree.GetNilNodePtr());
		EXPECT_EQ(tree.GetCount(), 0);
		tree.ClearDebugVector();

		if (i % (loop_count / 100) == 0)
		{
			PrintProgressBar(i, loop_count);
		}
	}
	PrintProgressBar(loop_count, loop_count);
	std::cout << std::endl;
}