#pragma once
#include "stdafx.h"
#define BUFFER_SIZE 1460
class CRingBuffer {
	friend class Session;
public:
	CRingBuffer();
	CRingBuffer(int iBufferSize);
	~CRingBuffer();

	int GetBufferSize(void) const;

	int GetUseSize(void) const;

	int GetFreeSize(void) const;

	int Enqueue(const char* chpData, int iSize);

	int Dequeue(char* chpDest, int iSize);

	// ThreadSafe ÇÔ¼ö
	int EnqueueLock(const char* chpData, int iSize);
	int DequeueLock(char* chpDest, int iSize);

	int Peek(char* chpDest, int iSize) const;

	void ClearBuffer(void);

	int DirectEnqueueSize(void) const;
	int DirectDequeueSize(void) const;

	int DirectEnqueueSizeLock(void);
	int DirectDequeueSizeLock(void);
	
	char* GetFrontBufferPtr(void);

	char* GetRearBufferPtr(void);
	char* GetBufferPtr(void);

	SRWLOCK* GetSRWLockPtr()
	{
		return &srwlock_;
	};

private:
	int MoveRear(int iSize);
	int MoveFront(int iSize);

private:
	int size_ = 0;
	int front_ = 0;
	int rear_ = 0;
	char* buffer_ = nullptr;
	SRWLOCK srwlock_ = {}; 
};