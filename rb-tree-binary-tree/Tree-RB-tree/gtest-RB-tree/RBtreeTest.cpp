#include "pch.h"
#include "RBtreeTest.h"

void RBtreeTest::GenerateRandomVector(int size, int numRange)
{
	inserted_keys.clear();
	std::uniform_int_distribution<int> dist(1, numRange);

	inserted_keys.reserve(size);
	for (int i = 0; i < size; i++)
	{
		inserted_keys.push_back(dist(rng));
	}
}

void RBtreeTest::InsertAll()
{
	auto it = inserted_keys.begin();
	while (it != inserted_keys.end())
	{
		int key = (*it);
		tree.Insert(key, 0);
		CheckRBtreeLogic(); // RB check
		++it;
	}
}
void RBtreeTest::DeleteAll()
{
	auto it = inserted_keys.begin();
	while (it != inserted_keys.end())
	{
		tree.Delete((*it));
		CheckRBtreeLogic(); // RB check
		++it;
	}
}
void RBtreeTest::SortVector(std::vector<int>& destVec)
{
	std::sort(destVec.begin(), destVec.end());
}

void RBtreeTest::SortDuplicate()
{
	auto new_end = std::unique(inserted_keys.begin(), inserted_keys.end());
	inserted_keys.erase(new_end, inserted_keys.end());
}

void RBtreeTest::ShuffleInsertedKeys()
{
	std::shuffle(inserted_keys.begin(), inserted_keys.end(), rng);
}

void RBtreeTest::PrintProgressBar(int current, int total)
{
	int bar_width = 50;
	float progress = static_cast<float>(current) / total;
	int pos = static_cast<int>(bar_width * progress);

	std::cout << "\r[";

	for (int i = 0; i < bar_width; ++i)
	{
		if (i < pos) std::cout << "=";
		else if (i == pos) std::cout << ">";
		else std::cout << " ";
	}

	std::cout << "] " << int(progress * 100.0) << " %" << std::flush;
}

void RBtreeTest::CheckRBtreeLogic()
{
	EXPECT_TRUE(RootIsBlack()); // 1. root가 블랙인가
	CheckRBtreeRecursive(tree.GetRootNode(),NodeColor::BLACK);
}

int RBtreeTest::CheckRBtreeRecursive(Node* curr, NodeColor parentColor)
{
	EXPECT_TRUE(curr->Color == NodeColor::BLACK || curr->Color == NodeColor::RED); // 2. 모든 노드가 색을 가지고 있는가
	if (curr == tree.GetNilNodePtr())
	{
		return 1;
	}
	EXPECT_NE(curr, nullptr); // 4. 모든 리프노드는 Nil노드인가
	
	// 5. 레드 노드의 자식이 블랙인가
	if (parentColor == NodeColor::RED)
	{
		EXPECT_EQ(curr->Color, NodeColor::BLACK);
	}
	
	int sumL = CheckRBtreeRecursive(curr->Left,curr->Color);
	int sumR = CheckRBtreeRecursive(curr->Right, curr->Color);

	EXPECT_EQ(sumL, sumR); // 3. 루트노드부터 리프노드까지 모든 경로의 블랙의 수는 같은가 -> 여기서는 현재 노드부터 리프노드까지의 모든 경로의 블랙의 수는 같은가를 검증한다
	return sumL;
}
 