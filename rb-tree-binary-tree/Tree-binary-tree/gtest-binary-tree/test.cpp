#include "pch.h"
#include "../Tree-binary-tree/BinaryTree.h"
#include <random>
class BinaryTreeTest : public ::testing::Test
{
protected:
	BinaryTree<int> tree;
	std::vector<int> inserted_keys;

	void GenerateRandomVector(int size = 1000)
	{
		inserted_keys.clear();
		//
		std::random_device rd;
		std::mt19937 rng(rd());

		std::uniform_int_distribution<int> dist(1, 100);

		inserted_keys.reserve(size);
		for (int i = 0; i < size; i++)
		{
			inserted_keys.push_back(dist(rng));
		}
	}

	void InsertAll()
	{
		auto it = inserted_keys.begin();
		while(it != inserted_keys.end())
		{
			tree.Insert((*it), 0);
			++it;
		}
	}

	void SortVector(std::vector<int> &destVec)
	{
		std::sort(destVec.begin(), destVec.end());
	}

	void SortDuplicate()
	{
		auto new_end = std::unique(inserted_keys.begin(), inserted_keys.end());
		inserted_keys.erase(new_end, inserted_keys.end());
	}


	// 반복 테스트 함수 (횟수, 벡터 크기, 테스트 이름 인자 등 필요하면 확장 가능)
	void RunInsertDeleteLoop(int repeatCount = 1000, int vectorSize = 1000)
	{
		for (int i = 0; i < repeatCount; ++i)
		{
			if (i % (repeatCount/10) == 0)
			{
				std::cout << "current loop count: " << i << "\n";
			}
			inserted_keys.clear();

			GenerateRandomVector(vectorSize);
			InsertAll();
			SortVector(inserted_keys);
			SortDuplicate();

			std::vector<int> inorder_keys;
			tree.GetSortedKeyArray(inorder_keys);

			EXPECT_EQ(inserted_keys, inorder_keys) << "Insert validation failed";

			// 랜덤 셔플 후 삭제
			std::random_device rd;
			std::mt19937 rng(rd());
			std::uniform_int_distribution<int> dist(1, 1000);
			std::shuffle(inserted_keys.begin(), inserted_keys.end(), rng);

			for (int key : inserted_keys)
				tree.Delete(key);

			EXPECT_EQ(tree.GetRootNode(), nullptr) << "Tree not empty after deletes";
			EXPECT_EQ(tree.GetCount(), 0) << "Count not zero after deletes";
		}
	}
};

#define TEST_INSERT_DELETE_STRESS(test_fixture, test_name, repeat_count, vector_size) \
TEST_F(test_fixture, test_name) { \
    RunInsertDeleteLoop(repeat_count, vector_size); \
}

// 기능 테스트
TEST_F(BinaryTreeTest, InsertRandomNodes) {
	GenerateRandomVector();
	InsertAll();
	SortVector(inserted_keys);
	SortDuplicate();
	std::vector<int> inorder_keys;
	tree.GetSortedKeyArray(inorder_keys);
	// 비교
	EXPECT_EQ(inserted_keys, inorder_keys);
}

TEST_F(BinaryTreeTest, DeleteSortedSequentialNodes)
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
	auto it = inserted_keys.begin();
	while (it != inserted_keys.end())
	{
		tree.Delete((*it));
		++it;
	}
	EXPECT_EQ(tree.GetRootNode(), nullptr);
	EXPECT_EQ(tree.GetCount(), 0);
}

TEST_F(BinaryTreeTest, DeleteSortedRandomShuffleNodes)
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
	// random 정렬
	std::random_device rd;
	std::mt19937 rng(rd());
	std::shuffle(inserted_keys.begin(), inserted_keys.end(), rng);
	//
	auto it = inserted_keys.begin();
	while (it != inserted_keys.end())
	{
		tree.Delete((*it));
		++it;
	}
	EXPECT_EQ(tree.GetRootNode(), nullptr);
	EXPECT_EQ(tree.GetCount(), 0);
}

// 스트레스 테스트
TEST_INSERT_DELETE_STRESS(BinaryTreeTest, StressInsertDelete_, 1000*100, 1000);
