//#include <gtest/gtest.h>
//#include "CLockFreeObjectPool.h"
//
//
//struct Node
//{
//    Node* TaggedNext;
//};
//
//static internal::CLockFreeNodeStack<Node*> gStack;
//const int THREAD_COUNT = 4;
//const int LOOP_COUNT = 10000;
//
//unsigned __stdcall PushPopThread(void* arg)
//{
//    for (int i = 0; i < LOOP_COUNT;i++)
//    {
//        for (int i = 0; i < 10000; i++)
//        {
//            gStack.Push(new Node);
//        }
//        for (int i = 0; i < 10000; i++)
//        {
//            Node* pNode;
//            gStack.Pop(pNode);
//        }
//    }
//    return 0;
//}
//
//TEST(CLockFreeNodeStack_Test, TaggedPointerFunctionTest)
//{
//    internal::CLockFreeNodeStack<Node*> stack;
//    uintptr_t ptr = 0x0000'7FFF'FFFF'FFFF;
//    uint64_t tag = 0x2;
//    PVOID taggedPtr = stack.CreateTaggedPtr64((Node*)ptr, tag);
//    EXPECT_EQ(taggedPtr, (Node*)0x0001'7FFF'FFFF'FFFF);
//    Node* cleanPtr = stack.GetCleanPtr64((Node*)taggedPtr);
//    EXPECT_EQ(cleanPtr, (Node*)ptr);
//}
//
//TEST(CLockFreeNodeStack_Test,DISABLED_ABA_ManualTest)
//{
//    std::vector<HANDLE> threads;
//
//    for (int i = 0; i < THREAD_COUNT; ++i)
//    {
//        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, PushPopThread, NULL, 0, NULL);
//        threads.push_back(hThread);
//    }
//
//    WaitForMultipleObjects((DWORD)threads.size(), threads.data(), TRUE, INFINITE);
//
//    for (HANDLE hThread : threads)
//    {
//        CloseHandle(hThread);
//    }
//}