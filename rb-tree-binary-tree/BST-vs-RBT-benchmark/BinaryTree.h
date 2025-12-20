#pragma once
#include <vector>
#include <iostream>
template <typename T>
class BinaryTree
{
public:
	// ------------
	// GDI 활용위해 public 적용
	// ------------
	struct Node
	{
		int Key;
		T Value;
		int Count;
		Node* Left = nullptr;
		Node* Right = nullptr;
		Node* Parent = nullptr; // 이진에선 필수X
	};

	BinaryTree() = default;
	~BinaryTree();

	Node* GetRootNode() { return root_; }
	void Insert(int key, T value);
	int Delete(int key);
	T GetValue(int key);
	bool Find(int key);
	void Print();
	void GetSortedKeyArray(std::vector<T>& destVec);
	int GetCount(void) { return count_; }

private:
	void SearchForPrint(Node* node);
	void SearchForVector(Node* node, std::vector<T>& destVec);
	void DestroyTree(Node*);

private:
	Node* root_ = nullptr;
	int count_ = 0;
};

template<typename T>
inline BinaryTree<T>::~BinaryTree()
{
	DestroyTree(root_);
}

template<typename T>
inline void BinaryTree<T>::Insert(int key, T value)
{
	Node** curr = &root_;
	Node* parent = nullptr;

	while (*curr != nullptr) {
		parent = *curr;
		if (key < (*curr)->Key) {
			curr = &((*curr)->Left);
		}
		else if (key > (*curr)->Key) {
			curr = &((*curr)->Right);
		}
		else {
			(*curr)->Count++; // 중복은 카운트로 간단히 처리
			return;
		}
	}

	*curr = new Node{ key,value,1,nullptr,nullptr,parent };
	++count_;
}

template<typename T>
inline int BinaryTree<T>::Delete(int key)
{
	Node** curr = &root_;
	Node* parent = nullptr;

	// search
	while (*curr) {
		if (key < (*curr)->Key) {
			parent = *curr;
			curr = &((*curr)->Left);
		}
		else if (key > (*curr)->Key) {
			parent = *curr;
			curr = &((*curr)->Right);
		}
		else {
			break;
		}
	}

	// *curr : 현재노드
	// curr : 현재노드를 가리키고 있는 부모의 멤버 노드 포인터
	if (*curr == nullptr) {
		return 0;
	}

	// case 1: 자식 노드가 2개 일때
	if ((*curr)->Left && (*curr)->Right) {
		Node** toDelete = &((*curr)->Right);
		Node* parent = *curr;
		while ((*toDelete)->Left != nullptr) {
			parent = *toDelete;
			toDelete = &((*toDelete)->Left);
		}

		(*curr)->Key = (*toDelete)->Key;
		(*curr)->Value = (*toDelete)->Value;
		(*curr)->Count = (*toDelete)->Count;

		// 삭제할 노드의 Right노드 유무에 따른 분기
		Node* child = nullptr;
		if ((*toDelete)->Right != nullptr) {
			child = (*toDelete)->Right;
			child->Parent = parent;
		}

		delete (*toDelete);
		*toDelete = child;

		--count_;
		return (*curr)->Count;
	}

	// case 2: 자식 노드가 단일이거나 없을때
	Node* child = nullptr;
	if ((*curr)->Left != nullptr) {
		child = (*curr)->Left;
	}
	else {
		child = (*curr)->Right;
	}

	int ret_count = (*curr)->Count;
	delete (*curr);

	if (child != nullptr) {
		child->Parent = parent;

	}
	*curr = child;

	--count_;
	return ret_count;
}



template<typename T>
inline T BinaryTree<T>::GetValue(int key)
{
	Node* node = root_;

	while (node != nullptr) {
		if (key < node->Key) {
			node = node->Left;
		}
		else if (key > node->Key) {
			node = node->Right;
		}
		else {
			// Find Key
			// 중복 허용시 변경필요
			return node->Value;
		}
	}
	return T{};
}
template<typename T>
inline bool BinaryTree<T>::Find(int key)
{
	Node* node = root_;

	while (node != nullptr) {
		if (key < node->Key) {
			node = node->Left;
		}
		else if (key > node->Key) {
			node = node->Right;
		}
		else {
			return true;
		}
	}
	return false;
}
template<typename T>
inline void BinaryTree<T>::SearchForPrint(Node* node)
{
	if (node == nullptr)
		return;

	SearchForPrint(node->Left);
	std::cout << node->Value << " ";
	SearchForPrint(node->Right);
}

template<typename T>
inline void BinaryTree<T>::DestroyTree(Node* node)
{
	// 후위 순회로 소멸시킨다
	// 참조 전에 소멸시키면 문제가 되기 때문
	if (node != nullptr) {
		DestroyTree(node->Left);
		DestroyTree(node->Right);
		delete node;
		return;
	}
}

template<typename T>
inline void BinaryTree<T>::Print()
{
	SearchForPrint(root_);
}

template<typename T>
inline void BinaryTree<T>::GetSortedKeyArray(std::vector<T>& destVec)
{
	SearchForVector(root_, destVec);
}

template<typename T>
inline void BinaryTree<T>::SearchForVector(Node* node, std::vector<T>& destVec)
{
	if (node == nullptr)
		return;

	// 중위 순회
	SearchForVector(node->Left, destVec);
	destVec.push_back(node->Key);
	SearchForVector(node->Right, destVec);
}
