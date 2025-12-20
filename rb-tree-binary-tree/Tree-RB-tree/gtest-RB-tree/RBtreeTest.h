#pragma once
#include "../Tree-RB-tree/RBtree.h"
#include <iostream>
#include <random>

// 레드블랙트리 검증 프로그램
// -----------
// 레드블랙트리 삽입, 삭제시 체크 포인트
// 1. root가 블랙인가
// 2. 모든 노드가 색을 가지고 있는가
// 3. 루트노드부터 리프노드까지 모든 경로의 블랙의 수는 같은가
// 4. 모든 리프노드는 Nil노드인가
// 5. 레드 노드의 자식이 블랙인가
// + 제대로 대소관계에 따라 정렬 되었는가 (기본 이진트리)
// -----------

class RBtreeTest : public ::testing::Test
{
protected:
	using Node = RBtree<int>::Node;
	using NodeColor = RBtree<int>::NODE_COLOR;

	RBtreeTest() : rng(std::random_device{}()) {}

	// 삽입용 벡터에 데이터 생성
	void GenerateRandomVector(int size = 1000, int numRange = 1000);
	// 벡터에서 트리에 데이터 삽입
	void InsertAll();
	// 트리 데이터 삭제
	void DeleteAll();
	// 파라미터 벡터를 정렬한다
	void SortVector(std::vector<int>& destVec);
	// 삽입용 벡터의 중복을 제거한다.
	void SortDuplicate();
	// 삽입용 벡터 내부를 셔플한다
	void ShuffleInsertedKeys();
	// 출력 관련
	void PrintProgressBar(int,int);
	// --------------------
	// 레드블랙트리용
	// --------------------
	void CheckRBtreeLogic();
	int CheckRBtreeRecursive(Node*,NodeColor);
	bool RootIsBlack() { return tree.GetRootNode()->Color == tree.BLACK; };

	RBtree<int> tree;
	std::vector<int> inserted_keys;	// 삽입용 벡터
	std::mt19937 rng;
};