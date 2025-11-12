//#pragma once
//#include <Windows.h>
//#include <cstdint>
//#include "CLockFreeObjectPool.h"
//namespace core
//{
//#define LOCK_FREE_QUEUE_DEBUG
//
//	const uint32_t DEBUG_LOG_MAX = 10000;
//	const uint32_t TAG_SHIFT_BIT = 64 - 17;
//	const uintptr_t TAG_MASK_ERASE = 0x0000'7FFF'FFFF'FFFF;
//
//	template <typename T>
//	class CLockFreeQueue
//	{
//	public:
//		CLockFreeQueue(long maxSize);
//		~CLockFreeQueue() = default;
//
//	private:
//		struct Node
//		{
//			T Data;
//			Node* TaggedNext;
//		};
//
//		enum eType : char
//		{
//			PUSH,
//			POP,
//		};
//
//		struct DebugData
//		{
//			uint64_t LoopCount;
//			eType Type;
//			DWORD ThreadId;
//			uint64_t LogIndex;
//			Node* OldTail;
//			Node* NextTail;
//			Node* OldHead;
//			Node* NextHead;
//			uint64_t InterlockedCount;
//			uint32_t Size;
//		};
//
//	public:
//		void Push(T data);
//		bool Pop(T& data);
//
//		inline Node* CreateTaggedPtr64(Node* pNode, uint64_t tag)
//		{
//			return reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(pNode) | (tag << TAG_SHIFT_BIT));
//		}
//		inline Node* GetCleanPtr64(Node* pTaggedNode)
//		{
//			return reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(pTaggedNode) & TAG_MASK_ERASE);
//		}
//	private:
//		void CreateDummyNode();
//
//	public:
//		uint64_t nodeTagCount_ = 0; // FOR TAG
//
//
//	private:
//#ifdef LOCK_FREE_QUEUE_DEBUG
//		uint64_t logIndex_ = 0;
//		uint64_t loopCount_ = 0;
//		uint64_t interlockedCount_ = 0;
//		DebugData log_[DEBUG_LOG_MAX] = {};
//		//uint64_t pushPopCount_ = 0;
//#endif
//	private:
//		CLockFreeObjectPool<Node> nodePool_;
//		
//		Node* taggedHead_ = nullptr;
//		Node* taggedTail_ = nullptr;
//
//		long size_ = 0;
//		long maxSize_;
//	};
//
//	template<typename T>
//	inline CLockFreeQueue<T>::CLockFreeQueue(long maxSize): maxSize_(maxSize)
//	{
//		CreateDummyNode();
//	}
//
//	template<typename T>
//	inline void CLockFreeQueue<T>::Push(T data)
//	{
//#ifdef LOCK_FREE_QUEUE_DEBUG
//		uint32_t size = 0;
//#endif
//		if (size_ > maxSize_) return;
//
//		Node* newNode = nodePool_.Alloc();
//		newNode->Data = data;
//		newNode->TaggedNext = nullptr;
//		uint64_t tag = InterlockedIncrement(&nodeTagCount_);
//		Node* taggedNode = CreateTaggedPtr64(newNode, tag);
//
//		while (true)
//		{
//#ifdef LOCK_FREE_QUEUE_DEBUG
//
//			auto loopCount1 = InterlockedIncrement(&loopCount_);
//#endif
//			Node* currentTaggedTail = taggedTail_;
//			Node* currentTail = GetCleanPtr64(currentTaggedTail);
//			//Node* nextTaggedTail = currentTail->TaggedNext;
//
//			if (currentTail->TaggedNext)
//			{
//				if (InterlockedCompareExchangePointer((PVOID*)&taggedTail_, currentTail->TaggedNext, currentTaggedTail) == (PVOID)currentTaggedTail)
//				{
//
//				}
//				else
//				{
//
//				}
//			}
//			else
//			{
//				// debug
//			}
//
//			
//			if (InterlockedCompareExchangePointer((PVOID*)&currentTail->TaggedNext, taggedNode, nullptr) == (PVOID)nullptr)
//			{
//				auto interlockedCount = InterlockedIncrement(&interlockedCount_);
//
//				if(InterlockedCompareExchangePointer((PVOID*)&taggedTail_, taggedNode, currentTaggedTail) == (PVOID)currentTaggedTail)
//				{
//
//				}
//				else
//				{
//					auto loopCount2 = InterlockedIncrement(&loopCount_);
//					printf("2번 실패\n");
//				}
//
//				auto retSize = InterlockedIncrement(&size_);
//
//	#ifdef LOCK_FREE_QUEUE_DEBUG
//					auto originIndex = InterlockedIncrement(&logIndex_);
//					auto index = (originIndex - 1) % DEBUG_LOG_MAX;
//					DebugData data{};
//					data.ThreadId = GetCurrentThreadId();
//					data.Type = eType::PUSH;
//					data.LoopCount = loopCount1;
//					data.NextTail = taggedNode;
//					data.OldTail = currentTaggedTail;
//					data.LogIndex = index;
//					data.InterlockedCount = interlockedCount;
//					data.Size = retSize;
//					log_[index] = data;
//	#endif
//					return;
//			}
//		}
//	}
//
//	template<typename T>
//	inline bool CLockFreeQueue<T>::Pop(T& data)
//	{
//		long size = InterlockedDecrement(&size_);
//		if (size < 0)
//		{
//			InterlockedIncrement(&size_);
//			return false;
//		}
//		while (1)
//		{
//#ifdef LOCK_FREE_QUEUE_DEBUG
//
//			auto loopCount = InterlockedIncrement(&loopCount_);
//#endif
//			Node* currentTaggedHead = taggedHead_;
//			Node* currentHead = GetCleanPtr64(currentTaggedHead);
//			Node* nextTaggedHead = currentHead->TaggedNext;
//
//			//if (nextTaggedHead == nullptr) // 또는 헤드와 테일이 더미노드인지, 사이즈가 0인지
//			//{
//			//	continue;
//			//}
//
//			Node* nextHead = GetCleanPtr64(nextTaggedHead);
//			data = nextHead->Data;
//
//			if (size == 0 && nextTaggedHead)
//			{
//				if (InterlockedCompareExchangePointer((PVOID*)&taggedTail_, nextTaggedHead, currentTaggedHead) == (PVOID)currentTaggedHead)
//				{
//
//				}
//				else
//				{
//					printf("2번카스 실패후 deque시 2번카스 재실행 실패\n");
//				}
//			}
//
//			if (InterlockedCompareExchangePointer((PVOID*)&taggedHead_, nextTaggedHead, currentTaggedHead) == (PVOID)currentTaggedHead)
//			{
//#ifdef LOCK_FREE_QUEUE_DEBUG
//				auto originIndex = InterlockedIncrement(&logIndex_);
//				auto index = (originIndex - 1) % DEBUG_LOG_MAX;
//				DebugData data{};
//				data.ThreadId = GetCurrentThreadId();
//				data.Type = eType::POP;
//				data.LoopCount = loopCount;
//				data.OldHead = currentTaggedHead;
//				data.NextHead = nextTaggedHead;
//				data.LogIndex = originIndex;
//				data.Size = size;
//				log_[index] = data;
//#endif
//				nodePool_.Free(currentHead);
//				return true;
//			}
//		}
//	}
//
//	template<typename T>
//	inline void CLockFreeQueue<T>::CreateDummyNode()
//	{
//		uint64_t tag = InterlockedIncrement(&nodeTagCount_);
//		taggedTail_ = nodePool_.Alloc();
//		taggedTail_->TaggedNext = nullptr; // tail_->TaggedNext와 동일
//		taggedTail_ = CreateTaggedPtr64(taggedTail_, tag);
//		taggedHead_ = taggedTail_;
//	}
//}