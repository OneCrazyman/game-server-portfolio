#include <gtest/gtest.h>
#include "Util/CRingBuffer.h"

TEST(RingBufferTest, Initialization) {
    CRingBuffer rbuf(1024);
    EXPECT_EQ(rbuf.GetBufferSize(), 1024);
    EXPECT_EQ(rbuf.GetUseSize(), 0);
    EXPECT_EQ(rbuf.GetFreeSize(), 1024);
}

TEST(RingBufferTest, EnqueueDequeue) {
    CRingBuffer rbuf(100);
    char src[50] = "Hello, RingBuffer!";
    char dst[50] = { 0 };

    int enq = rbuf.Enqueue(src, static_cast<int>(strlen(src)));
    EXPECT_EQ(enq, strlen(src));
    EXPECT_EQ(rbuf.GetUseSize(), enq);
    EXPECT_EQ(rbuf.GetFreeSize(), 100 - enq);

    int deq = rbuf.Dequeue(dst, 50);
    EXPECT_EQ(deq, enq);
    EXPECT_EQ(strcmp(src, dst), 0);
}

TEST(RingBufferTest, PeekDoesNotAdvance) {
    CRingBuffer rbuf(100);
    char src[] = "Test Peek";
    char dst[50] = { 0 };

    rbuf.Enqueue(src, static_cast<int>(strlen(src)));
    rbuf.Peek(dst, static_cast<int>(strlen(src)));
    EXPECT_EQ(strcmp(src, dst), 0);
    EXPECT_EQ(rbuf.GetUseSize(), static_cast<int>(strlen(src)));
}

TEST(RingBufferTest, ClearBuffer)
{
    CRingBuffer rbuf(256);
    char src[50] = "Clear Buffer Test";
    rbuf.Enqueue(src, static_cast<int>(strlen(src)));
    EXPECT_EQ(rbuf.GetUseSize(), static_cast<int>(strlen(src)));
    EXPECT_EQ(rbuf.GetFreeSize(), 256 - static_cast<int>(strlen(src)));
    rbuf.ClearBuffer();
    EXPECT_EQ(rbuf.GetUseSize(), 0);
    EXPECT_EQ(rbuf.GetFreeSize(), 256);
}

TEST(RingBufferTest, EnqueueDequeueFullBuffer) {
    const int size = 1024;
    CRingBuffer rbuf(size);
    char* src = new char[size];
    char* dst = new char[size];

    for (int i = 0; i < size; ++i) {
        src[i] = i % 256;
    }
    
    int total = 0;
    while (total < size) {
        int toWrite = rand() % rbuf.GetFreeSize() + 1;
        total += rbuf.Enqueue(src + total, toWrite);
    }
    EXPECT_EQ(rbuf.GetUseSize(), size);

    int readTotal = 0;
    while (readTotal < size) {
        int toRead = rand() % rbuf.GetUseSize() + 1;
        readTotal += rbuf.Dequeue(dst + readTotal, toRead);
    }

    EXPECT_EQ(memcmp(src, dst, size), 0);
    delete[] src;
    delete[] dst;
}

// 
TEST(RingBufferTest, FreeSizeBugTest)
{
    const int size = 1000;
    CRingBuffer rbuf(size);
    EXPECT_EQ(rbuf.DirectEnqueueSize(), rbuf.GetFreeSize());
}

TEST(RingBufferTest, DirectEnqueueSize_IncludesBufferEnd_WhenFrontIsNonZero)
{
    const int size = 10;
    CRingBuffer rbuf(size);
    EXPECT_EQ(rbuf.GetBufferSize(), size);

    // rear_ = 5
    char in[5] = {};
    int rear = rbuf.Enqueue(in, 5);

    // front_ = 2
    char out[2];
    int front = rbuf.Dequeue(out, 2);

    EXPECT_EQ(rbuf.GetUseSize(), 3);
    int frontFreeSize = front - 1; // queue full½Ã ÇÑÄ­ ºñ¿ò
    EXPECT_EQ(rbuf.DirectEnqueueSize() + frontFreeSize, rbuf.GetFreeSize());
}