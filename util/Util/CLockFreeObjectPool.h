//#pragma once
//#include <Windows.h>
//#include <iostream>
//#include <cstdint>
//#pragma comment(lib, "winmm.lib")
//
//namespace internal
//{
//	template <typename T>
//	class CLockFreeNodeStack;
//}
//
//namespace core
//{
//	//-------------------
//	// 디버깅 옵션 -> 성능 비용 발생
//	//-------------------
//	#define MEMORY_POOL_DEBUG
//
//	template <typename T>
//	class CLockFreeObjectPool
//	{
//	public:
//		// Parameters : (int)OjbectNum : 최초 오브젝트 기본 개수
//		// Parameters : (bool)placementNew : true이면 alloc마다 생성자 호출
//		CLockFreeObjectPool(int ObjectNum = 0, bool placementNew = false);
//		~CLockFreeObjectPool() = default;
//
//		// 반환되어 스택에 있는 노드에서 T* 형태로 반환,
//		// 비어있을시 새로운 노드를 동적할당하여 같은 형태로 제공한다
//		// Return: (T*)동적할당된 노드의 (T)Data 주소
//		T* Alloc();
//
//		// Alloc받았던 T주소를 프리리스트에 반환한다
//		// Parameters: (T*)
//		void Free(T* pT);
//
//		uint32_t GetAllocCount() { return allocCount_; }
//	private:
//		void ReportError(unsigned int, unsigned int);
//		
//	private:
//		//-------------------
//		// 동적할당되어 관리되는 노드 단위
//		// 침범 확인을 위한 오버,언더플로우를 검사하게 설계되었다.
//		// 객체 인스턴스마다 고유의 식별자 부여
//		//-------------------
//		struct Node
//		{
//			unsigned int Id = 0;	// 인스턴스 식별자
//			T Data;
//			Node* TaggedNext = nullptr;
//	#ifdef MEMORY_POOL_DEBUG
//			Node* PoolNext = nullptr;
//			unsigned int Guard = 0;
//	#endif
//		};
//
//	private:
//		//-------------------
//		// allocHead_ : FreeList의 Alloc에서 새로 할당된 노드들을 추적하는 head 노드 포인터, 헤드를 따라가면 모든 생성된 노드를 추적할 수 있다.
//		//-------------------
//	#ifdef MEMORY_POOL_DEBUG
//		Node* allocHead_ = nullptr;
//	#endif
//		internal::CLockFreeNodeStack<Node*> stack_;
//
//		unsigned int allocCount_ = 0;
//
//		unsigned int objectPoolId_ = 0;
//		inline static unsigned int objectPoolIdCount_ = 0;
//
//		bool placementNew_ = false;
//	};
//
//	template<typename T>
//	inline CLockFreeObjectPool<T>::CLockFreeObjectPool(int ojbectNum, bool placementNew)
//	{
//		// objectNum TODO
//		objectPoolId_ = InterlockedIncrement(&objectPoolIdCount_);
//		placementNew_ = placementNew;
//	}
//
//	template<typename T>
//	inline T* CLockFreeObjectPool<T>::Alloc()
//	{
//		Node* node = nullptr;
//		if (stack_.Pop(node) == false)
//		{
//			node = new Node; // need fix -> smart ptr?
//			node->Id = objectPoolId_;
//	#ifdef MEMORY_POOL_DEBUG
//			node->Guard = objectPoolId_;	
//			node->PoolNext = allocHead_;
//			allocHead_ = node;
//	#endif
//			InterlockedIncrement(&allocCount_);
//			return &node->Data;
//		}
//		else
//		{
//			// ctor
//			if (placementNew_ == true)
//			{
//				new (&node->Data) T();
//			}
//			return &(node->Data);
//		}
//	}
//
//	template<typename T>
//	inline void CLockFreeObjectPool<T>::Free(T* pData)
//	{   
//		size_t offset = offsetof(Node, Data);
//		Node* freeNode = reinterpret_cast<Node*>(reinterpret_cast<LPBYTE>(pData) - offset);
//		//-------------------
//		// 데이터 유효 검증
//		//-------------------
//		// Data앞부분 메모리 영역 침범과 인스턴스 고유 id값 검증
//		if (freeNode->Id != objectPoolId_)
//		{
//			ReportError(objectPoolId_, freeNode->Id);
//		}
//	#ifdef MEMORY_POOL_DEBUG
//		// Data뒷부분 메모리 영역 침범과 중복 Free 검증
//		unsigned int guard = freeNode->Guard;
//		if (guard != objectPoolId_)
//		{
//			ReportError(objectPoolId_, guard);
//		}
//	#endif
//		// dtor
//		if (placementNew_ == true)
//		{
//			pData->~T();
//		}
//		stack_.Push(freeNode);
//	}
//
//	template<typename T>
//	inline void CLockFreeObjectPool<T>::ReportError(unsigned int expected, unsigned int actual)
//	{
//		std::cerr << "[ERROR] CLockFreeObjectPool _ Id mismatch, expected: " << expected << " actual: " << actual << "\n";
//		exit(1);
//	}
//}
//
//namespace internal
//{
//#define LOCK_FREE_NODE_STACK_DEBUG
//
//	const uint32_t DEBUG_LOG_MAX = 1000;
//	const uint32_t TAG_SHIFT_BIT = 64 - 17;
//	const uintptr_t TAG_MASK_ERASE = 0x0000'7FFF'FFFF'FFFF;
//	const uintptr_t TAG_MASK_USE = ~(0x0000'7FFF'FFFF'FFFF);
//
//	template <typename T>
//	class CLockFreeNodeStack
//	{
//	public:
//		CLockFreeNodeStack();
//		~CLockFreeNodeStack();
//
//		enum eType : char
//		{
//			PUSH,
//			POP,
//		};
//
//	private:
//		struct DebugData
//		{
//			uint64_t LoopCount;
//			uint64_t LogIndex;
//			DWORD ThreadId;
//			eType Type;
//			T OldTop;
//			T NewTop;
//			uint64_t OldId;
//			uint64_t NewId;
//		};
//
//	public:
//		void Push(T pNode);
//		bool Pop(T& pNode);
//
//		inline T CreateTaggedPtr64(T pNode, uint64_t tag)
//		{
//			return reinterpret_cast<T>(reinterpret_cast<uintptr_t>(pNode) | (tag << TAG_SHIFT_BIT));
//		}
//		inline T GetCleanPtr64(T pTagNode)
//		{
//			return reinterpret_cast<T>(reinterpret_cast<uintptr_t>(pTagNode) & TAG_MASK_ERASE);
//		}
//
//	private:
//		void AssertSystemRequirements(void);
//		void FreeStackMemory(void);
//
//	private:
//#ifdef LOCK_FREE_NODE_STACK_DEBUG
//		uint64_t logIndex_ = 0;
//		uint64_t loopCount_ = 0;
//		DebugData log_[DEBUG_LOG_MAX] = {};
//		uint64_t pushPopCount_ = 0;
//#endif
//
//	private:
//		T taggedTop_ = nullptr;
//		uint64_t nodeTagCount_ = 0; // for tag
//	};
//
//	template<typename T>
//	inline CLockFreeNodeStack<T>::CLockFreeNodeStack()
//	{
//		AssertSystemRequirements();
//	}
//
//	template<typename T>
//	inline CLockFreeNodeStack<T>::~CLockFreeNodeStack()
//	{
//		FreeStackMemory();
//	}
//
//	template<typename T>
//	inline void CLockFreeNodeStack<T>::Push(T pNode)
//	{
//#ifdef LOCK_FREE_NODE_STACK_DEBUG
//		uint64_t tmpCount = InterlockedIncrement(&pushPopCount_);
//#endif
//		uint64_t id = InterlockedIncrement(&nodeTagCount_);
//		T taggedNode = CreateTaggedPtr64(pNode, id);
//
//		while (true)
//		{
//#ifdef LOCK_FREE_NODE_STACK_DEBUG
//			auto loopCount = InterlockedIncrement(&loopCount_);
//#endif
//			T oldTaggedTop = taggedTop_;
//			pNode->TaggedNext = oldTaggedTop;
//			if (InterlockedCompareExchangePointer((PVOID*)&taggedTop_, taggedNode, oldTaggedTop) == (PVOID)oldTaggedTop)
//			{
//#ifdef LOCK_FREE_NODE_STACK_DEBUG
//				auto originIndex = InterlockedIncrement(&logIndex_);
//				auto index = (originIndex - 1) % DEBUG_LOG_MAX;
//				DebugData data{};
//				data.ThreadId = GetCurrentThreadId();
//				data.NewTop = taggedNode;
//				data.OldTop = oldTaggedTop;
//				data.Type = eType::PUSH;
//				data.NewId = id;
//				data.LogIndex = index;
//				data.LoopCount = loopCount;
//				log_[index] = data;
//#endif
//				return;
//			}
//		}
//	}
//
//	template<typename T>
//	inline bool CLockFreeNodeStack<T>::Pop(T& pNode)
//	{
//#ifdef LOCK_FREE_NODE_STACK_DEBUG
//		uint64_t tmpCount = InterlockedDecrement(&pushPopCount_);
//#endif
//		while (1)
//		{
//#ifdef LOCK_FREE_NODE_STACK_DEBUG
//			auto loopCount = InterlockedIncrement(&loopCount_);
//#endif
//			T currentTaggedTop = taggedTop_;
//			if (currentTaggedTop == nullptr)
//			{
//				return false;
//			}
//			T oldTop = GetCleanPtr64(currentTaggedTop);
//
//			T newTaggedTop = oldTop->TaggedNext;
//			pNode = oldTop;
//
//			if (InterlockedCompareExchangePointer((PVOID*)&taggedTop_, newTaggedTop, currentTaggedTop) == (PVOID)currentTaggedTop)
//			{
//#ifdef LOCK_FREE_NODE_STACK_DEBUG
//				auto originIndex = InterlockedIncrement(&logIndex_);
//				auto index = (originIndex - 1) % DEBUG_LOG_MAX;
//				DebugData data{};
//				data.ThreadId = GetCurrentThreadId();
//				data.OldTop = currentTaggedTop;
//				data.NewTop = newTaggedTop;
//				data.Type = eType::POP;
//				data.LogIndex = originIndex;
//				data.LoopCount = loopCount;
//				log_[index] = data;
//#endif
//				return true;
//			}
//		}
//	}
//	template<typename T>
//	inline void CLockFreeNodeStack<T>::AssertSystemRequirements(void)
//	{
//		static_assert(sizeof(PVOID) == 8, "This lock-free stack requires a 64-bit architecture\n");
//
//		SYSTEM_INFO si;
//		GetSystemInfo(&si);
//		if (reinterpret_cast<uintptr_t>(si.lpMaximumApplicationAddress) & TAG_MASK_USE)
//		{
//			printf("The application address space is too large. LockFreeStack is not running on this system\n");
//			exit(1);
//		}
//	}
//
//	template<typename T>
//
//	inline void CLockFreeNodeStack<T>::FreeStackMemory(void)
//	{
//		T currentTop = GetCleanPtr64(taggedTop_);
//		while (currentTop != nullptr)
//		{
//			T deleteToNode = currentTop;
//			currentTop = GetCleanPtr64(currentTop->TaggedNext);
//
//			delete deleteToNode;
//		}
//	}
//}