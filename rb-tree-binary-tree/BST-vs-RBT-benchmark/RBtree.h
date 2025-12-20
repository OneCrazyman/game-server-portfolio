#pragma once
#include <vector>
#include <iostream>
template <typename T>
class RBtree
{
public:
	// ------------
	// GDI 활용위해 public 적용
	// ------------
	enum NODE_COLOR
	{
		BLACK = 0,
		RED
	};

	struct Node
	{
		int Key;   // Key , Value
		T Value;
		int Count;

		Node* Left;
		Node* Right;
		Node* Parent;

		NODE_COLOR Color;

	};

	RBtree();
	~RBtree();

	Node* GetRootNode() { return root_; }
	void Insert(int key, T value);
	int Delete(int key);
	T GetValue(int key);
	bool Find(int key);
	void Print();
	void GetSortedKeyArray(std::vector<T>& destVec);
	int GetCount(void) { return count_; };
	int GetNodeCount(int key);
	Node* GetNilNodePtr(void) { return &Nil_; };
	// 파라미터 노드 기준으로 우회전, 좌회전
	void RotateRight(Node*);
	void RotateLeft(Node*);

private:
	void ToggleColor(Node* node)
	{
		node->Color = static_cast<NODE_COLOR>(static_cast<int>(node->Color) ^ 1);
	};
	Node* NewNode(int key, T value, Node* parent, NODE_COLOR color)
	{
		++count_;
		return new Node{ key,value,1,&Nil_,&Nil_,parent,color };
	}
	void SearchForPrint(Node* node);
	void SearchForVector(Node* node, std::vector<T>& destVec);
	void DestroyTree(Node*);
	void AttachToParent(Node*);
	void RedBlackBalancing(Node*, Node*);
private:
	Node* root_ = &Nil_;
	Node Nil_ = {};
	int count_ = 0;
};

template<typename T>
inline RBtree<T>::RBtree()
{
	Nil_.Color = BLACK; // Nil노드
}

template<typename T>
inline RBtree<T>::~RBtree()
{
	DestroyTree(root_);
}

template<typename T>
inline void RBtree<T>::Insert(int key, T value)
{
	Node* curr = root_;
	Node* parent = &Nil_;

	// 첫 삽입은 루트 노드로 생성
	if (curr == &Nil_) {
		curr = NewNode(key, value, &Nil_, BLACK);
		root_ = curr;
		return;
	}

	// 검색
	while (curr != &Nil_) {
		parent = curr;
		if (key < curr->Key) {
			curr = curr->Left;
		}
		else if (key > curr->Key) {
			curr = curr->Right;
		}
		else {
			++(curr->Count);
			return;
		}
	}

	curr = NewNode(key, value, parent, RED);
	AttachToParent(curr);

	// 부모 블랙, 정상 삽입
	if (parent->Color == BLACK) return;

	// --------------
	// 부모 레드 상황 -> 분기
	// --------------
	while (curr->Parent->Color == RED) {
		Node* parent = curr->Parent;
		Node* grand = parent->Parent;
		bool isLeft = (parent == grand->Left);
		Node* uncle = isLeft ? (grand->Right) : (grand->Left);
		Node* case2 = isLeft ? (parent->Right) : (parent->Left);
		// case 1: 부모 레드, 삼촌 레드
		if (uncle->Color == RED) {
			parent->Color = BLACK;	// 부모
			uncle->Color = BLACK;	// 삼촌
			grand->Color = RED;		// 조부모

			curr = grand;			// 조부모로 이동
			continue;
		}

		// case 2: 부모 레드, 삼촌 블랙, 나는 부모의 오른쪽 레드 (왼쪽기준)
		else if (uncle->Color == BLACK && curr == case2) {
			// Parent를 기준으로 회전하여 바깥쪽으로 노드를 세운다.
			if (isLeft) {
				RotateLeft(parent);
				curr = curr->Left;
			}
			else {
				RotateRight(parent);
				curr = curr->Right;
			}
			continue;
		}
		// case 3: 부모 레드, 삼촌 블랙, 나는 부모 왼쪽 레드 (왼쪽기준)
		else {
			parent->Color = BLACK;
			grand->Color = RED;
			if (isLeft)
				RotateRight(grand);
			else
				RotateLeft(grand);
			break;
		}
	}
	if (curr == root_) {
		curr->Color = BLACK;
	}
	// case 3에서 root기준 회전했는지 검사
	// else if -> root가 아닐때만 조건 검사 가능
	else if (curr->Parent->Parent == &Nil_) {
		root_ = curr->Parent;
	}
}

template<typename T>
inline int RBtree<T>::Delete(int key)
{
	Node* curr = root_;
	Node* parent = &Nil_;

	// search
	while (curr != &Nil_) {
		if (key < curr->Key) {
			parent = curr;
			curr = curr->Left;
		}
		else if (key > curr->Key) {
			parent = curr;
			curr = curr->Right;
		}
		else {
			break;
		}
	}

	// 맞는키가 없거나, 루트가 Nil일때
	if (curr == &Nil_) {
		return 0;
	}

	// case 1: 자식 노드가 2개 일때
	if (curr->Left != &Nil_ && curr->Right != &Nil_) {
		Node* toDelete = curr->Right;
		Node* parent = curr;
		while (toDelete->Left != &Nil_) {
			parent = toDelete;
			toDelete = toDelete->Left;
		}

		curr->Key = toDelete->Key;
		curr->Value = toDelete->Value;
		curr->Count = toDelete->Count;

		// 삭제할 노드의 Right노드 유무에 따른 분기
		Node* child = &Nil_;
		if (toDelete->Right != nullptr) {
			child = toDelete->Right;
			child->Parent = parent;
		}

		if (parent->Left == toDelete)
			parent->Left = child;
		else
			parent->Right = child;

		RedBlackBalancing(toDelete, child);

		delete toDelete;
		--count_;
		return curr->Count;
	}

	// case 2: 자식 노드가 단일이거나 없을때
	Node* child = &Nil_;
	if (curr->Left != &Nil_) {
		child = curr->Left;
	}
	else {
		child = curr->Right;
	}

	// Nil이여도 Nil부모에 잠시 등록하여 사용할것이다
	child->Parent = parent;	// parent 등록 

	if (curr == root_) {
		root_ = child;
		root_->Color = BLACK;
	}
	else {
		if (parent->Left == curr)	// child 등록
			parent->Left = child;
		else
			parent->Right = child;
	}

	int ret_count = curr->Count;
	RedBlackBalancing(curr, child);
	delete curr;
	--count_;

	return ret_count;
}



template<typename T>
inline T RBtree<T>::GetValue(int key)
{
	// loop
	Node* node = root_;

	while (node != &Nil_) {
		if (key < node->Key) {
			node = node->Left;
		}
		else if (key > node->Key) {
			node = node->Right;
		}
		else {	// Find Key
			// 중복 허용시 변경필요
			return node->Value;
		}
	}
	return T{};
}
template<typename T>
inline bool RBtree<T>::Find(int key)
{
	// loop
	Node* node = root_;

	while (node != &Nil_) {
		if (key < node->Key) {
			node = node->Left;
		}
		else if (key > node->Key) {
			node = node->Right;
		}
		else {	// Find Key
			// 중복 허용시 변경필요
			return true;
		}
	}
	return false;
}

template<typename T>
inline void RBtree<T>::SearchForPrint(Node* node)
{
	if (node == nullptr)
		return;

	// 중위 순회
	SearchForPrint(node->Left);
	std::cout << node->Value << " ";
	SearchForPrint(node->Right);
}

template<typename T>
inline void RBtree<T>::DestroyTree(Node* node)
{
	// 후위 순회로 소멸시킨다
	// 참조 전에 소멸시키면 문제가 되기 때문
	if (node != &Nil_) {
		DestroyTree(node->Left);
		DestroyTree(node->Right);
		delete node;
		return;
	}
}

template<typename T>
inline void RBtree<T>::AttachToParent(Node* N)
{
	if (N->Key < N->Parent->Key) {
		N->Parent->Left = N;
	}
	else {
		N->Parent->Right = N;
	}
}

template<typename T>
inline void RBtree<T>::RedBlackBalancing(Node* curr, Node* child)
{
	// case 1: 삭제노드가 레드
	if (curr->Color == RED) {	// 정상 삭제
		return;
	}

	// ------------
	// 삭제노드가 블랙인 경우, 사라진 블랙 -1에 대해 밸런싱해야한다
	// Loop가 필요하다
	// ------------
	while (true) {
		if (child == root_) {
			return;
		}
		// case 2: 삭제노드 블랙, 자식이 레드인 경우
		if (child->Color == RED) {
			child->Color = BLACK;	// 자식을 블랙으로 맞춤으로써 밸런싱한다
			return;
		}

		Node* parent = child->Parent;
		bool is_left = parent->Left == child ? true : false;
		Node* sibling = is_left ? parent->Right : parent->Left;

		// case 3: 형제가 레드인 경우
		if (sibling->Color == RED) {
			sibling->Color = BLACK;
			parent->Color = RED;
			if (is_left)
				RotateLeft(parent);
			else
				RotateRight(parent);
			// 형제가 블랙인 케이스로 유도
			continue;
		}

		// case 4: 형제가 블랙, 형제 양쪽 자식이 블랙인 경우
		if (sibling->Left->Color == BLACK && sibling->Right->Color == BLACK) {
			sibling->Color = RED;
			child = parent;
			// case 2나 다른케이스로 유도
			continue;
		}

		Node* in = is_left ? sibling->Left : sibling->Right;
		Node* out = is_left ? sibling->Right : sibling->Left;

		// case 5: '', 형제의 안쪽자식이 레드, 바깥쪽이 블랙
		if (in->Color == RED && out->Color == BLACK) {
			in->Color = BLACK;
			sibling->Color = RED;
			if (is_left)
				RotateRight(sibling);
			else
				RotateLeft(sibling);
			// case 6으로 유도
			continue;
		}

		// case 6: '', 형제의 바깥쪽자식이 레드
		else // 결국 case 6이 되어야 parent수준에서 밸런싱이 가능하다
		{
			sibling->Color = parent->Color;
			parent->Color = BLACK;
			out->Color = BLACK;
			if (is_left)
				RotateLeft(parent);
			else
				RotateRight(parent);
			return;
		}
	}
}

template<typename T>
inline void RBtree<T>::RotateRight(Node* N)
{
	Node* P = N->Parent;
	Node* L = N->Left;
	Node* LR = L->Right;

	if (P != &Nil_) {
		if (P->Left == N) {
			P->Left = L;
		}
		else {
			P->Right = L;
		}
	}
	else {
		root_ = L;
	}

	L->Parent = P;	// P가 null 가능 -> 이때 L은 루트

	L->Right = N;
	N->Parent = L;

	N->Left = LR;
	if (LR != &Nil_) {
		LR->Parent = N;
	}
}

template<typename T>
inline void RBtree<T>::RotateLeft(Node* N)
{
	Node* P = N->Parent;
	Node* R = N->Right;
	Node* RL = R->Left;

	if (P != &Nil_) {
		if (P->Left == N) {
			P->Left = R;
		}
		else {
			P->Right = R;
		}
	}
	else {
		root_ = R;
	}

	R->Parent = P;	// P가 null 가능 -> 이때 R은 루트

	R->Left = N;
	N->Parent = R;

	N->Right = RL;
	if (RL != &Nil_) {
		RL->Parent = N;
	}
}

template<typename T>
inline void RBtree<T>::Print()
{
	SearchForPrint(root_);
}


// 정렬된 벡터 얻기
template<typename T>
inline void RBtree<T>::GetSortedKeyArray(std::vector<T>& destVec)
{
	SearchForVector(root_, destVec);
}

template<typename T>
inline int RBtree<T>::GetNodeCount(int key)
{
	Node* node = root_;

	while (node != &Nil_) {
		if (key < node->Key) {
			node = node->Left;
		}
		else if (key > node->Key) {
			node = node->Right;
		}
		else {	// Find Key
			// 중복 허용시 변경필요
			return node->Count;
		}
	}
	return 0;
}

template<typename T>
inline void RBtree<T>::SearchForVector(Node* node, std::vector<T>& destVec)
{
	if (node == &Nil_)
		return;

	// 중위 순회
	SearchForVector(node->Left, destVec);
	destVec.push_back(node->Key);
	SearchForVector(node->Right, destVec);
}