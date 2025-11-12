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