#include <gtest/gtest.h>
//#include "CLockFreeQueue.h"
//
//
//const int LOOP_COUNT = 10'000;
//const int PUSH_POP_COUNT = 5;
//
//class CLockFreeQueue_Test : public ::testing::Test
//{
//protected:
//
//    void SetUp() override
//    {
//        queue_ = new core::CLockFreeQueue<int>(LOOP_COUNT);
//        pushCountTotal_ = 0;
//        popCountTotal_ = 0;
//    }
//
//    void TearDown() override
//    {
//        delete queue_;
//        queue_ = nullptr;
//    }
//
//    static unsigned int __stdcall PushPopThread(void* lpParam);
//    static unsigned int __stdcall PushPopLoopThread(void* lpParam);
//    static unsigned int __stdcall PopThread(void* lpParam);
//    static unsigned int __stdcall PushThread(void* lpParam);
//
//    uint64_t pushCountTotal_ = 0;
//    uint64_t popCountTotal_ = 0;
//    uint64_t pushSumTotal_ = 0;
//    uint64_t popSumTotal_ = 0;
//
//    core::CLockFreeQueue<int>* queue_ = nullptr;
//};
//
//TEST_F(CLockFreeQueue_Test, SinglePushPopTest)
//{
//    const int LOOP_NUM = 100;
//    for (int j = 0; j < LOOP_NUM; j++)
//    {
//        for (int i = 0;i < PUSH_POP_COUNT;i++)
//        {
//            queue_->Push(i);
//        }
//        int data;
//        for (int i = 0;i < PUSH_POP_COUNT;i++)
//        {
//            queue_->Pop(data);
//            EXPECT_EQ(data, i);
//        }
//    }
//
//    for (int i = 0;i < LOOP_NUM;i++)
//    {
//        queue_->Push(i);
//    }
//    int data;
//    for (int i = 0;i < LOOP_NUM;i++)
//    {
//        queue_->Pop(data);
//        EXPECT_EQ(data, i);
//    }
//}
//
//TEST_F(CLockFreeQueue_Test, ConcurrentPushPop_DataIntegrityTest)
//{
//    //while (1)
//    {
//    const int thread_num = 4;
//    std::vector<HANDLE> threads;
//
//    for (int i = 0; i < thread_num; ++i)
//    {
//        threads.push_back(reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, PushPopThread, this, 0, NULL)));
//    }
//
//    for (auto t : threads)
//    {
//        WaitForSingleObject(t, INFINITE);
//        CloseHandle(t);
//    }
//
//    EXPECT_EQ(pushCountTotal_, popCountTotal_);
//    EXPECT_EQ(pushSumTotal_, popSumTotal_);
//    }
//}
//
//TEST_F(CLockFreeQueue_Test, DISABLED_InfiniteLoopPushPopTest)
//{
//    const int thread_num = 4;
//    std::vector<HANDLE> threads;
//
//    this->pushCountTotal_ = 0;
//    this->popCountTotal_ = 0;
//
//    for (int i = 0; i < thread_num; ++i)
//    {
//        threads.push_back(reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, PushPopLoopThread, this, 0, NULL)));
//    }
//
//    for (auto t : threads)
//    {
//        WaitForSingleObject(t, INFINITE);
//        CloseHandle(t);
//    }
//
//    const int totalPushes = pushCountTotal_;
//    const int totalPopCount = popCountTotal_;
//    EXPECT_EQ(totalPushes, totalPopCount);
//}
//
//
//
//unsigned int __stdcall CLockFreeQueue_Test::PushPopThread(void* lpParam)
//{
//    auto* fixture = static_cast<CLockFreeQueue_Test*>(lpParam);
//    uint64_t pushCount = 0;
//    uint64_t popCount = 0;
//    uint64_t pushSum = 0;
//    uint64_t popSum = 0;
//            
//    for (size_t j = 0; j < LOOP_COUNT; j++)
//    {
//        for (size_t i = 0; i < PUSH_POP_COUNT; ++i)
//        {
//            fixture->queue_->Push(i);
//            pushCount++;
//            pushSum += i;
//        }
//        for (size_t i = 0; i < PUSH_POP_COUNT; ++i)
//        {
//            int retVal;
//            if (fixture->queue_->Pop(retVal))
//            {
//                popCount++;
//                popSum += retVal;
//            }
//        }
//    }
//
//    InterlockedExchangeAdd(&fixture->pushCountTotal_, pushCount);
//    InterlockedExchangeAdd(&fixture->popCountTotal_, popCount);
//    InterlockedExchangeAdd(&fixture->pushSumTotal_, pushSum);
//    InterlockedExchangeAdd(&fixture->popSumTotal_, popSum);
//    return 0;
//}
//
//unsigned int __stdcall CLockFreeQueue_Test::PushPopLoopThread(void* lpParam)
//{
//    auto* fixture = static_cast<CLockFreeQueue_Test*>(lpParam);
//    uint64_t pushCount = 0;
//    uint64_t popCount = 0;
//    int PUSH_POP_COUNT = 4;
//    //int mainLoopCount = 0;
//    while(1)
//    {
//        for (size_t i = 0; i < PUSH_POP_COUNT; ++i)
//        {
//            fixture->queue_->Push(i);
//            pushCount++;
//        }
//        for (size_t i = 0; i < PUSH_POP_COUNT; ++i)
//        {
//            int retVal;
//            if (fixture->queue_->Pop(retVal))
//            {
//                popCount++;
//            }
//        }
//        InterlockedExchangeAdd(&fixture->pushCountTotal_, pushCount);
//        InterlockedExchangeAdd(&fixture->popCountTotal_, popCount);
//        //mainLoopCount++;
//        //if (mainLoopCount % 10000 == 0)
//        //{
//        //    printf("push:%ll,pop:%ll\n", pushCount, popCount);
//        //}
//    }
//    return 0;
//}
//
//unsigned int __stdcall CLockFreeQueue_Test::PopThread(void* lpParam)
//{
//    auto* fixture = static_cast<CLockFreeQueue_Test*>(lpParam);
//    for (size_t i = 0; i < PUSH_POP_COUNT; )
//    {
//        int popData;
//        if (fixture->queue_->Pop(popData))
//        {
//            i++;
//        }
//        else
//            continue;
//    }
//    return 0;
//}
//
//unsigned int __stdcall CLockFreeQueue_Test::PushThread(void* lpParam)
//{
//    auto* fixture = static_cast<CLockFreeQueue_Test*>(lpParam);
//    for (size_t i = 0; i < PUSH_POP_COUNT; i++)
//    {
//        fixture->queue_->Push(i);
//    }
//    return 0;
//}
