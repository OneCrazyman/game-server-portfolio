//#include <gtest/gtest.h>
//#include "CLockFreeStack.h"
//
//
//const int LOOP_COUNT = 100'000;
//
//class CLockFreeStack_Test : public ::testing::Test 
//{
//protected:
//
//   void SetUp() override 
//   {
//       stack_ = new core::CLockFreeStack<int>();
//   }
//
//   void TearDown() override 
//   {
//       delete stack_;
//       stack_ = nullptr;
//   }
//
//   static unsigned int __stdcall PushPopThread(void* lpParam)
//   {
//       auto* fixture = static_cast<CLockFreeStack_Test*>(lpParam);
//       uint64_t pushCount = 0;
//       uint64_t popCount = 0;
//       //while (1)
//       //{
//           for (size_t i = 0; i < LOOP_COUNT; ++i)
//           {
//               fixture->stack_->Push(i);
//               pushCount++;
//           }
//           for (size_t i = 0; i < LOOP_COUNT; ++i)
//           {
//               int retVal;
//               if (fixture->stack_->Pop(retVal))
//               {
//                   popCount++;
//               }
//           }
//       //    printf("%d\n", fixture->queue_->nodePool_.GetAllocCount());
//       //}
//       InterlockedExchangeAdd(&fixture->pushCount_, pushCount);
//       InterlockedExchangeAdd(&fixture->popCount_, popCount);
//       return 0;
//   }
//
//
//   static unsigned int __stdcall PopThread(void* lpParam)
//   {
//       auto* fixture = static_cast<CLockFreeStack_Test*>(lpParam);
//       for (size_t i = 0; i < LOOP_COUNT; )
//       {
//           int popData;
//           if (fixture->stack_->Pop(popData))
//           {
//               i++;
//           }
//           else
//               continue;
//       }
//       return 0;
//   }
//
//   static unsigned int __stdcall PushThread(void* lpParam)
//   {
//       auto* fixture = static_cast<CLockFreeStack_Test*>(lpParam);
//       for (size_t i = 0; i < LOOP_COUNT; i++)
//       {
//           fixture->stack_->Push(i);
//       }
//       return 0;
//   }
//
//   uint64_t pushCount_ = 0;
//   uint64_t popCount_ =0;
//   core::CLockFreeStack<int>* stack_ = nullptr;
//};
//
//TEST_F(CLockFreeStack_Test, ConcurrentPushPopTest)
//{
//   const int thread_num = 4;
//   std::vector<HANDLE> threads;
//
//   for (int i = 0; i < thread_num; ++i) 
//   {
//       threads.push_back(reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, PushPopThread, this, 0, NULL)));
//   }
//
//   for (auto t : threads)
//   {
//       WaitForSingleObject(t, INFINITE);
//       CloseHandle(t);
//   }
//
//   const int totalPushes = pushCount_;
//   const int totalPopCount = popCount_;
//   EXPECT_EQ(totalPushes, totalPopCount);
//}
//
//
//TEST_F(CLockFreeStack_Test, SinglePushPopTest)
//{
//    stack_->Push(10);
//    stack_->Push(20);
//    stack_->Push(30);
//    int data;
//    stack_->Pop(data);
//    EXPECT_EQ(data, 30);
//    stack_->Pop(data);
//    EXPECT_EQ(data, 20);
//    stack_->Pop(data);
//    EXPECT_EQ(data, 10);
//}