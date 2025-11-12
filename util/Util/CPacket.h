#pragma once
#include <Windows.h>
#include <type_traits>

class CPacket
{
	friend class Session;
public:

	enum class en_PACKET
	{
		eBUFFER_DEFAULT = 1400
	};

	enum class en_ERROR : char
	{
		NONE = 100,
		BUFFER_FULL = 101,
		BUFFER_EMPTY = 102,
		COPY_INSTANCE_FAIL = 103,
	};

	CPacket();
	CPacket(int BufferSize);
	CPacket(const CPacket& other);

	virtual	~CPacket();

	void	Clear(void);

	int	GetBufferSize(void) { return buffer_size_; }

	int	GetDataSize(void) { return data_size_; }

	char* GetBufferPtr(void) { return buffer_; }

	CPacket& operator = (const CPacket& clSrcPacket);

	template <typename T>
	typename std::enable_if<
		std::is_same<T, char>::value ||
		std::is_same<T, unsigned char>::value ||
		std::is_same<T, short>::value ||
		std::is_same<T, unsigned short>::value ||
		std::is_same<T, int>::value ||
		std::is_same<T, unsigned int>::value ||
		std::is_same<T, long>::value ||
		std::is_same<T, unsigned long>::value ||
		std::is_same<T, long long>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, DWORD>::value ||
		std::is_same<T, unsigned int>::value ||
		std::is_same<T, __int64>::value ||
		std::is_same<T, __int32>::value ||
		std::is_same<T, __int16>::value,
		CPacket&
	>::type
		operator << (T value);

	template <typename T>
	typename std::enable_if<
		std::is_same<T, char>::value ||
		std::is_same<T, unsigned char>::value ||
		std::is_same<T, short>::value ||
		std::is_same<T, unsigned short>::value ||
		std::is_same<T, int>::value ||
		std::is_same<T, unsigned int>::value ||
		std::is_same<T, long>::value ||
		std::is_same<T, unsigned long>::value ||
		std::is_same<T, long long>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, DWORD>::value ||
		std::is_same<T, unsigned int>::value ||
		std::is_same<T, __int64>::value ||
		std::is_same<T, __int32>::value ||
		std::is_same<T, __int16>::value,
		CPacket&
	>::type
		operator >> (T& value);

	int		GetData(char* pDest, int Size);
	int		PutData(char* pSrc, int SrcSize);

	en_ERROR GetLastError(void)
	{
		return last_error_;
	}

private:
	int		MoveWritePos(int Size);
	int		MoveReadPos(int Size);

	void CopyFrom(const CPacket& other);

private:
	int	buffer_size_;
	int	data_size_ = 0;	// 현재 버퍼에 사용중인 사이즈.
	en_ERROR last_error_ = en_ERROR::NONE;
	char* buffer_;
	char* readpos_;
	char* writepos_;
};

inline CPacket::CPacket()
{
	buffer_size_ = (int)en_PACKET::eBUFFER_DEFAULT;
	buffer_ = new char[buffer_size_];
	readpos_ = buffer_;
	writepos_ = buffer_;
}

inline CPacket::CPacket(int iBufferSize)
{
	buffer_size_ = iBufferSize;
	buffer_ = new char[buffer_size_];
	readpos_ = buffer_;
	writepos_ = buffer_;
}


inline CPacket::~CPacket()
{
	delete[] buffer_;
}

inline void CPacket::Clear(void)
{
	data_size_ = 0;
	readpos_ = buffer_;
	writepos_ = buffer_;
	last_error_ = en_ERROR::NONE;
}

inline int CPacket::MoveWritePos(int Size)
{
	if (Size > buffer_size_ - (writepos_ - buffer_)) {
		return 0;
	}
	writepos_ += Size;
	data_size_ += Size;
	return Size;
}

inline int CPacket::MoveReadPos(int Size)
{
	if (Size > data_size_) {
		return 0;
	}
	readpos_ += Size;
	data_size_ -= Size;
	return Size;
}

inline CPacket& CPacket::operator=(const CPacket& clSrcPacket)
{
	if (this == &clSrcPacket) {
		return *this;
	}
	if (buffer_size_ < clSrcPacket.data_size_) {
		last_error_ = en_ERROR::COPY_INSTANCE_FAIL;
		return *this;
	}
	CopyFrom(clSrcPacket);
	return *this;
}

inline CPacket::CPacket(const CPacket& clSrcPacket)
{
	buffer_size_ = clSrcPacket.buffer_size_;
	buffer_ = new char[buffer_size_];
	CopyFrom(clSrcPacket);
}

inline void CPacket::CopyFrom(const CPacket& clSrcPacket)
{
	data_size_ = clSrcPacket.data_size_;
	memcpy_s(buffer_, buffer_size_, clSrcPacket.buffer_, clSrcPacket.data_size_);
	readpos_ = buffer_ + (clSrcPacket.readpos_ - clSrcPacket.buffer_);
	writepos_ = buffer_ + (clSrcPacket.writepos_ - clSrcPacket.buffer_);
	last_error_ = clSrcPacket.last_error_;
}

inline int CPacket::GetData(char* pDest, int Size)
{
	if (Size > data_size_) {
		return 0;
	}
	memcpy_s(pDest, Size, readpos_, Size);
	readpos_ += Size;
	data_size_ -= Size;
	return Size;
}

inline int CPacket::PutData(char* pSrc, int SrcSize)
{
	if (SrcSize > buffer_size_ - (writepos_ - buffer_)) {
		last_error_ = en_ERROR::BUFFER_FULL;
		return 0;
	}
	memcpy_s(writepos_, (buffer_size_ - (writepos_ - buffer_)), pSrc, SrcSize);
	writepos_ += SrcSize;
	data_size_ += SrcSize;
	return SrcSize;
}

template <typename T>
typename std::enable_if<
	std::is_same<T, char>::value ||
	std::is_same<T, unsigned char>::value ||
	std::is_same<T, short>::value ||
	std::is_same<T, unsigned short>::value ||
	std::is_same<T, int>::value ||
	std::is_same<T, unsigned int>::value ||
	std::is_same<T, long>::value ||
	std::is_same<T, unsigned long>::value ||
	std::is_same<T, long long>::value ||
	std::is_same<T, float>::value ||
	std::is_same<T, double>::value ||
	std::is_same<T, DWORD>::value ||
	std::is_same<T, unsigned int>::value ||
	std::is_same<T, __int64>::value ||
	std::is_same<T, __int32>::value ||
	std::is_same<T, __int16>::value,
	CPacket&
>::type
inline CPacket::operator<<(T value)
{
	// 이 조건 해당시 UB 발생가능
	if (sizeof(value) > buffer_size_ - (writepos_ - buffer_)) {
		last_error_ = en_ERROR::BUFFER_FULL;
		return *this;
	}
	memcpy_s(writepos_, buffer_size_ - (writepos_ - buffer_), (char*)&value, sizeof(value));
	writepos_ += sizeof(value);
	data_size_ += sizeof(value);
	return *this;
}

template <typename T>
typename std::enable_if<
	std::is_same<T, char>::value ||
	std::is_same<T, unsigned char>::value ||
	std::is_same<T, short>::value ||
	std::is_same<T, unsigned short>::value ||
	std::is_same<T, int>::value ||
	std::is_same<T, unsigned int>::value ||
	std::is_same<T, long>::value ||
	std::is_same<T, unsigned long>::value ||
	std::is_same<T, long long>::value ||
	std::is_same<T, float>::value ||
	std::is_same<T, double>::value ||
	std::is_same<T, DWORD>::value ||
	std::is_same<T, unsigned int>::value ||
	std::is_same<T, __int64>::value ||
	std::is_same<T, __int32>::value ||
	std::is_same<T, __int16>::value,
	CPacket&
>::type
inline CPacket::operator>>(T& value)
{
	// 이 조건 해당시 UB 발생가능
	if (sizeof(value) > data_size_) {
		last_error_ = en_ERROR::BUFFER_EMPTY;
		return *this;
	}
	memcpy_s((char*)&value, sizeof(T), readpos_, sizeof(T));
	readpos_ += sizeof(T);
	data_size_ -= sizeof(T);
	return *this;
}