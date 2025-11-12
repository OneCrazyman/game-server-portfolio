#pragma once
#include <stddef.h>
#include <windows.h>
#include <iostream>
#include <stdexcept>
#pragma comment(lib, "winmm.lib")

#define MEMORY_POOL_DEBUG	// 디버깅용 기능을 포함

template <typename T>
class ObjectPool
{
public:
	// Parameters : (int)OjbectNum : 최초 오브젝트 기본 개수, (bool)placementNew -> true이면 alloc마다 생성자 호출
	ObjectPool(int ObjectNum = 0, bool placementNew = false);

	// 기본적으로 소멸자로 모든객체 자원의 해제를 기대하지 않는다. (프리리스트를 일부러 소멸시키지 않기 때문에 그렇게 설계)
	~ObjectPool();

	// 반환되어 스택에 있는 노드에서 T* 형태로 반환,
	// 비어있을시 새로운 노드를 동적할당하여 같은 형태로 제공한다.
	// Return: (T*)동적할당된 노드의 (T)Data 주소.
	T* Alloc();

	// Alloc받았던 T주소를 프리리스트에 반환한다.
	// Parameters: (T*)T타입주소
	void Free(T* pT);

private:
	void FreeStackMemory(void);
	void ReportError(unsigned int, unsigned int);
private:
	//-------------------
	// 동적할당되어 관리되는 노드 단위
	// 침범 확인을 위한 오버,언더플로우를 검사하게 설계되었다.
	// 객체 인스턴스마다 고유의 식별자 부여
	//-------------------
	struct Node
	{
		unsigned int Id;	// 인스턴스 식별자
		T Data;
#ifdef MEMORY_POOL_DEBUG
		Node* PoolNext = nullptr;
#endif
		Node* StackNext = nullptr;
	};

private:
	//-------------------
	// stack_top_  : 스택의 최상단 노드포인터. 기본적으로 Free()되면 stack에 push된다. 
	// alloc_head_ : FreeList의 Alloc에서 새로 할당된 노드들을 추적하는 head 노드 포인터, 헤드를 따라가면 모든 생성된 노드를 추적할 수 있다.
	//-------------------
	Node* stack_top_ = nullptr;
#ifdef MEMORY_POOL_DEBUG
	Node* alloc_head_ = nullptr;
#endif

	unsigned int alloc_count_ = 0;
	unsigned int stack_count_ = 0;

	unsigned int objectPoolId_;
	inline static int objectPoolId_count_ = 1;

	bool placement_new_ = false;
};

template<typename T>
inline ObjectPool<T>::ObjectPool(int ojbectNum, bool placementNew)
{
	// objectNum TODO
	objectPoolId_ = objectPoolId_count_++;
	placement_new_ = placementNew;
}

template<typename T>
inline ObjectPool<T>::~ObjectPool()
{
	FreeStackMemory();
}

template<typename T>
inline T* ObjectPool<T>::Alloc()
{
	if (stack_top_ == nullptr) {
		Node* new_node = new Node;
		new_node->Id = this->objectPoolId_;
#ifdef MEMORY_POOL_DEBUG
		*reinterpret_cast<unsigned int*>(&new_node->StackNext) = this->objectPoolId_;
		new_node->PoolNext = alloc_head_;
		alloc_head_ = new_node;
#endif
		++alloc_count_;
		return &new_node->Data;
	}
	else {	// 스택의 stack_top_에서 T* 반환
		Node* used_node = stack_top_;
		if (placement_new_ == true) {
			new (&used_node->Data) T();
		}
		stack_top_ = stack_top_->StackNext;
#ifdef MEMORY_POOL_DEBUG
		*reinterpret_cast<unsigned int*>(&used_node->StackNext) = this->objectPoolId_;
#endif
		--stack_count_;
		return &used_node->Data;
	}
}

template<typename T>
inline void ObjectPool<T>::Free(T* pData)
{   
	size_t offset = offsetof(Node, Data);
	Node* free_node = reinterpret_cast<Node*>(reinterpret_cast<char*>(pData) - offset);

	// Data 전방 메모리 영역 침범과 인스턴스 고유 id값 검증
	if (free_node->Id != objectPoolId_) {
		throw std::runtime_error("Free: id is invalid\n");

	}
#ifdef MEMORY_POOL_DEBUG
	// Data 후방 메모리 영역 침범과 중복 Free 검증
	unsigned int cookie = *reinterpret_cast<unsigned int*>(&free_node->StackNext);
	if (cookie != objectPoolId_) {
		throw std::runtime_error("Free: id is invalid\n");
	}
#endif
	if (placement_new_ == true) {
		pData->~T();
	}
	free_node->StackNext = stack_top_;
	stack_top_ = free_node;
	++stack_count_;
}

template<typename T>
inline void ObjectPool<T>::FreeStackMemory()
{
	while (stack_top_ != nullptr) {
		Node* delete_to_node = stack_top_;
		stack_top_ = stack_top_->StackNext;

		delete delete_to_node;
	}
}

template<typename T>
inline void ObjectPool<T>::ReportError(unsigned int expected, unsigned int actual)
{
	std::cerr << "[ERROR] ObjectFreeList _ Id mismatch, expected: " << expected << " actual: " << actual << "\n";
}