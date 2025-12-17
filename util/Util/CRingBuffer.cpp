#include "CRingBuffer.h"

CRingBuffer::CRingBuffer() : CRingBuffer(BUFFER_SIZE) {};

CRingBuffer::CRingBuffer(int iBufferSize)
{
	size_ = iBufferSize + 1; // 사용자에게 iBufferSize만큼 최대 free_size_를 제공하기 위해 +1
	buffer_ = new char[size_];
	InitializeSRWLock(&srwlock_);
}

CRingBuffer::~CRingBuffer()
{
	delete[] buffer_;
}

int CRingBuffer::GetBufferSize(void) const
{
	return (size_ - 1);
}

int CRingBuffer::GetUseSize(void) const
{
	if (rear_ >= front_)
		return (rear_ - front_);
	else
		return (size_ - front_ + rear_);
}

int CRingBuffer::GetFreeSize(void) const
{
	if (rear_ >= front_)
		return (size_ - (rear_ - front_) - 1);
	else
		return (front_ - rear_ - 1);
}

int CRingBuffer::Enqueue(const char* chpData, int iSize)
{
	if (iSize > GetFreeSize()) {
		return 0;
	}
	for (int i = 0; i < iSize; i++) {
		buffer_[rear_] = chpData[i];
		rear_ = (rear_ + 1) % size_;
	}
	return iSize;
}

int CRingBuffer::Dequeue(char* chpDest, int iSize)
{
	int useSize = GetUseSize();
	if (useSize == 0 || iSize <= 0) {
		return 0;
	}
	if (iSize > useSize) {
		iSize = useSize;
	}
	for (int i = 0; i < iSize; i++) {
		chpDest[i] = buffer_[front_];
		front_ = (front_ + 1) % size_;
	}

	return iSize;
}

int CRingBuffer::EnqueueLock(const char* chpData, int iSize)
{
	if (iSize > GetFreeSize()) {
		return 0;
	}
	AcquireSRWLockExclusive(&srwlock_);
	for (int i = 0; i < iSize; i++) {
		buffer_[rear_] = chpData[i];
		rear_ = (rear_ + 1) % size_;
	}
	ReleaseSRWLockExclusive(&srwlock_);
	return iSize;
}
int CRingBuffer::DequeueLock(char* chpDest, int iSize)
{
	int useSize = GetUseSize();
	if (useSize == 0) {
		return 0;
	}
	if (iSize > useSize) {
		iSize = useSize;
	}
	AcquireSRWLockExclusive(&srwlock_);
	for (int i = 0; i < iSize; i++) {
		chpDest[i] = buffer_[front_];
		front_ = (front_ + 1) % size_;
	}
	ReleaseSRWLockExclusive(&srwlock_);
	return iSize;
}

int CRingBuffer::Peek(char* chpDest, int iSize) const
{
	int tmp_front = front_;
	int useSize = GetUseSize();
	if (useSize == 0) {
		return 0;
	}
	if (iSize > useSize) {
		iSize = useSize;
	}
	for (int i = 0; i < iSize; i++) {
		chpDest[i] = buffer_[tmp_front];
		tmp_front = (tmp_front + 1) % size_;
	}
	return iSize;
}

void CRingBuffer::ClearBuffer(void)
{
	rear_ = front_;
}

int CRingBuffer::DirectEnqueueSize(void) const
{
	int result;
	if (rear_ >= front_) {
		if (front_ == 0) // full일때 한칸 비워있어야 한다. 
			return (size_ - rear_ - 1); 
		else 
			return (size_ - rear_);
	}
	else {
		return (front_ - rear_ - 1);
	}
}

int CRingBuffer::DirectEnqueueSizeLock(void)
{
	AcquireSRWLockShared(&srwlock_);
	int result = DirectEnqueueSize();
	ReleaseSRWLockShared(&srwlock_);
	return result;
}

int CRingBuffer::DirectDequeueSize(void) const
{
	int result;
	if (rear_ >= front_) {
		result = rear_ - front_;
	}
	else {
		result = size_ - front_;
	}

	return result;
}

int CRingBuffer::DirectDequeueSizeLock(void)
{
	AcquireSRWLockShared(&srwlock_);
	int result = DirectDequeueSize();
	ReleaseSRWLockShared(&srwlock_);
	return result;
}

int CRingBuffer::MoveRear(int iSize)
{
	if (iSize > 0 && iSize > GetFreeSize()) return 0;
	if (iSize < 0 && (-iSize) > GetUseSize()) return 0;
	rear_ = (rear_ + iSize) % size_;
	return iSize;
}

int CRingBuffer::MoveFront(int iSize)
{
	if (iSize > 0 && iSize > GetUseSize()) return 0;
	if (iSize < 0 && (-iSize) > GetFreeSize()) return 0;
	front_ = (front_ + iSize) % size_;
	return iSize;
}

char* CRingBuffer::GetFrontBufferPtr(void)
{
	return &buffer_[front_];
}

char* CRingBuffer::GetRearBufferPtr(void)
{
	return &buffer_[rear_];
}

char* CRingBuffer::GetBufferPtr(void)
{
	return buffer_;
}