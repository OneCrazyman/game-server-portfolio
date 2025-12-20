// BST_ VS RBT_벤치마크

#include <vector>

#include "BinaryTree.h"
#include "RBTree.h"

#define ENABLED_PROFILER
#include "Util/Profiler.h"
#include "DataSetMaker.h"

int main()
{
	//const int DATA_SIZE = 10'000;
	const int LOOP = 20;

	DataSetMaker dsm;

	std::vector<int> sizes = { 100,500,1'000,5'000,10'000 };

	for (int i = 0; i < sizes.size(); i++) {
		const int DATA_SIZE = sizes[i];

		const auto sortedSet = dsm.GetSortedVector(DATA_SIZE);		// 1~n 정렬된 입력셋 (BST에겐 최악의 케이스)
		const auto randomSet = dsm.GetRandomVector(DATA_SIZE, 10);	// 1~n의 값이 랜덤으로 정렬된 입력셋

		// memory warmup
		{
			BinaryTree<int> bst;
			RBtree<int> rbt;
			for (int i = 0; i < sortedSet.size(); i++) {
				bst.Insert(sortedSet[i], 0);
				rbt.Insert(sortedSet[i], 0);
			}
		}

		for (int i = 0;i < LOOP;i++) {

			// --------------------
			// Insert : Sorted
			// --------------------
			{
				BinaryTree<int> bst;
				CREATE_PROFILE_TAG("BST_ Insert: sorted");
				for (int i = 0; i < sortedSet.size(); i++) {
					bst.Insert(sortedSet[i], 0);
				}
			}
			{
				RBtree<int> rbt;
				CREATE_PROFILE_TAG("RBT_ Insert: sorted");
				for (int i = 0; i < sortedSet.size(); i++) {
					rbt.Insert(sortedSet[i], 0);
				}
			}

			// --------------------
			// Insert : Random
			// --------------------
			{
				BinaryTree<int> bst;
				CREATE_PROFILE_TAG("BST_ Insert: random");
				for (int i = 0; i < randomSet.size(); i++) {
					bst.Insert(randomSet[i], 0);
				}
			}
			{
				RBtree<int> rbt;
				CREATE_PROFILE_TAG("RBT_ Insert: random");
				for (int i = 0; i < randomSet.size(); i++) {
					rbt.Insert(randomSet[i], 0);
				}
			}

			// --------------------
			// Search : Sorted
			// --------------------
			{
				BinaryTree<int> bst;
				for (int i = 0; i < sortedSet.size(); i++) {
					bst.Insert(sortedSet[i], 0);
				}
				CREATE_PROFILE_TAG("BST_ Search: sorted");
				for (int i = 0; i < sortedSet.size(); i++) {
					if (!bst.Find(sortedSet[i])) std::cout << "not find\n";
				}
			}
			{
				RBtree<int> rbt;
				for (int i = 0; i < sortedSet.size(); i++) {
					rbt.Insert(sortedSet[i], 0);
				}
				CREATE_PROFILE_TAG("RBT_ Search: sorted");
				for (int i = 0; i < sortedSet.size(); i++) {
					if (!rbt.Find(sortedSet[i])) std::cout << "not find\n";
				}
			}
			// --------------------
			// Search : Random
			// --------------------
			{
				BinaryTree<int> bst;
				for (int i = 0; i < randomSet.size(); i++) {
					bst.Insert(randomSet[i], 0);
				}
				CREATE_PROFILE_TAG("BST_ Search: random");
				for (int i = 0; i < randomSet.size(); i++) {
					if (!bst.Find(randomSet[i])) std::cout << "not find\n";
				}
			}
			{
				RBtree<int> rbt;
				for (int i = 0; i < randomSet.size(); i++) {
					rbt.Insert(randomSet[i], 0);
				}
				CREATE_PROFILE_TAG("RBT_ Search: random");
				for (int i = 0; i < randomSet.size(); i++) {
					if (!rbt.Find(randomSet[i])) std::cout << "not find\n";
				}
			}
		}

		std::string filename = "benchmark_" + std::to_string(DATA_SIZE) + ".txt";
		Profiler::DataOutText(filename.c_str());
		Profiler::Clear();
	}

	return 0;
}