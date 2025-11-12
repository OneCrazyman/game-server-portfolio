//#pragma once
//#include <Windows.h>
//#include <cstdint>
//#include "CLockFreeObjectPool.h"
//namespace core
//{
//#define LOCK_FREE_STACK_DEBUG
//
//	const uint32_t DEBUG_LOG_MAX = 1000;
//	const uint32_t TAG_SHIFT_BIT = 64 - 17;
//	const uintptr_t TAG_MASK_ERASE = 0x0000'7FFF'FFFF'FFFF;
//
//	template <typename T>
//	class CLockFreeStack
//	{
//	public:
//		CLockFreeStack() = default;
//		~CLockFreeStack() = default;
//
//		enum eType : char
//		{
//			PUSH,
//			POP,
//		};
//
//	private:
//		struct Node
//		{
//			T Data;
//			Node* TaggedNext;
//		};
//
//		struct DebugData
//		{
//			uint64_t LoopCount;
//			uint64_t LogIndex;
//			DWORD ThreadId;
//			eType Type;
//			Node* OldTop;
//			Node* NewTop;
//			uint64_t OldId;
//			uint64_t NewId;
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
//		inline Node* GetCleanPtr64(Node* pTagNode)
//		{
//			return reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(pTagNode) & TAG_MASK_ERASE);
//		}
//
//	private:
//#ifdef LOCK_FREE_STACK_DEBUG
//		uint64_t logIndex_ = 0;
//		uint64_t loopCount_ = 0;
//		DebugData log_[DEBUG_LOG_MAX] = {};
//		uint64_t pushPopCount_ = 0;
//#endif
//
//	private:
//		CLockFreeObjectPool<Node> nodePool_;
//
//		Node* taggedTop_ = nullptr;
//		uint64_t nodeTagCount_ = 0; // for tag
//	};
//
//
//	template<typename T>
//	inline void CLockFreeStack<T>::Push(T data)
//	{
//#ifdef LOCK_FREE_STACK_DEBUG
//		uint64_t tmpCount = InterlockedIncrement(&pushPopCount_);
//#endif
//		Node* newNode = nodePool_.Alloc();
//		uint64_t id = InterlockedIncrement(&nodeTagCount_);
//		Node* taggedNode = CreateTaggedPtr64(newNode, id);
//		newNode->Data = data;
//
//		while (true)
//		{
//#ifdef LOCK_FREE_STACK_DEBUG
//			auto loopCount = InterlockedIncrement(&loopCount_);
//#endif
//			Node* oldTaggedTop = taggedTop_;
//			newNode->TaggedNext = oldTaggedTop;
//			if (InterlockedCompareExchangePointer((PVOID*)&taggedTop_, taggedNode, oldTaggedTop) == (PVOID)oldTaggedTop)
//			{
//#ifdef LOCK_FREE_STACK_DEBUG
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
//	inline bool CLockFreeStack<T>::Pop(T& data)
//	{
//#ifdef LOCK_FREE_STACK_DEBUG
//		uint64_t tmpCount = InterlockedDecrement(&pushPopCount_);
//#endif
//		while (1)
//		{
//#ifdef LOCK_FREE_STACK_DEBUG
//			auto loopCount = InterlockedIncrement(&loopCount_);
//#endif
//			Node* currentTaggedTop = taggedTop_;
//			if (currentTaggedTop == nullptr)
//			{
//				return false;
//			}
//			Node* currentTop = GetCleanPtr64(currentTaggedTop);
//
//			Node* newTaggedTop = currentTop->TaggedNext;
//			data = currentTop->Data;
//
//			if (InterlockedCompareExchangePointer((PVOID*)&taggedTop_, newTaggedTop, currentTaggedTop) == (PVOID)currentTaggedTop)
//			{
//#ifdef LOCK_FREE_STACK_DEBUG
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
//				nodePool_.Free(currentTop);
//				return true;
//			}
//		}
//	}
//}