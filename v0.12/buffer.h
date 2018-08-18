#ifndef BUFFER_H
#define BUFFER_H

#include <algorithm>
#include <vector>
#include <string.h>
#include "socketops.h"

class Buffer
{
public:
	static const size_t kPrepend = 8;
	static const size_t kInitialSize = 1024;

	explicit Buffer(size_t initial_size = kInitialSize)
		: buffer_(kPrepend + kInitialSize),
		  read_index_(kPrepend),
		  write_index_(kPrepend) {}

	void Swap(Buffer& rhs)
	{
		buffer_.swap(rhs.buffer_);
		std::swap(read_index_, rhs.read_index_);
		std::swap(write_index_, rhs.write_index_);
	}

	size_t GetReadableSize() const { return write_index_ - read_index_; }
	size_t GetWritableSize() const { return buffer_.size() - write_index_; }
	size_t GetPrependableSize() const { return read_index_; }

	const char* GetReadablePtr() const { return GetBegin() + read_index_; }
	const char* GetWritablePtr() const { return GetBegin() + write_index_; }
	char* GetWritablePtr() { return GetBegin() + write_index_; }

	const char* FindCRLF() const 
	{
		const char* crlf = std::search(GetReadablePtr(), GetWritablePtr(), kCRLF, kCRLF + 2);
		return crlf == GetWritablePtr() ? NULL : crlf;
	}

	const char* FindCRLF(const char* start_ptr) const 
	{
		const char* crlf = std::search(start_ptr, GetWritablePtr(), kCRLF, kCRLF + 2);
		return crlf == GetWritablePtr() ? NULL : crlf;
	}

	const char* FindEOL() const
	{
		const void* eol = memchr(GetReadablePtr(), '\n', GetReadableSize());
		return static_cast<const char*>(eol);
	}

	const char* FindEOL(const char* start_ptr) const
	{
		const void* eol = memchr(start_ptr, '\n', GetWritablePtr() - start_ptr);
		return static_cast<const char*>(eol);
	}

	void Retrieve(size_t len)
	{
		if (len < GetReadableSize())
		{
			read_index_ += len;
		}
		else // len == GetReadableSize()
		{
			RetrieveAll();
		}
	}

	void RetrieveUntil(const char* end)
	{
		Retrieve(end - GetReadablePtr());
	}

	void RetrieveInt64()
	{
		Retrieve(sizeof(int64_t));
	}

	void RetrieveInt32()
	{
		Retrieve(sizeof(int32_t));
	}

	void RetrieveInt16()
	{
		Retrieve(sizeof(int16_t));
	}

	void RetrieveInt8()
	{
		Retrieve(sizeof(int8_t));
	}

	void RetrieveAll()
	{
		read_index_ = kPrepend;
		write_index_ = kPrepend;
	}

	std::string RetrieveAllAsString()
	{
		return RetrieveAsString(GetReadableSize());
	}

	std::string RetrieveAsString(size_t len)
	{
		std::string result(GetReadablePtr(), len);
		Retrieve(len);
		return result;
	}

	void EnsureWritable(size_t len)
	{
		if (GetWritableSize() < len)
		{
			MakeSpace(len);
		}
	}

	// void HasWritten(size_t len)
	// {
	// 	write_index_ += len;
	// }

	void Append(const char* data, size_t len)
	{
		if (GetWritableSize() < len)
		{
			MakeSpace(len);
		}
		std::copy(data, data + len, GetWritablePtr());
		write_index_ += len;
	}

	void Append(const void* data, size_t len)
	{
		Append(static_cast<const char*>(data), len);
	}

	void AppendInt64(int64_t x)
	{
		int64_t be64 = socketops::HostToNetwork64(x);
		Append(&be64, sizeof(be64));
	}

	void AppendInt32(int32_t x)
	{
		int64_t be32 = socketops::HostToNetwork32(x);
		Append(&be32, sizeof(be32));
	}

	void AppendInt16(int16_t x)
	{
		int64_t be16 = socketops::HostToNetwork16(x);
		Append(&be16, sizeof(be16));
	}

	void AppendInt8(int8_t x)
	{
		Append(&x, sizeof(x));
	}

	int64_t ReadInt64()
	{
		int64_t result = 0;
		memcpy(&result, GetReadablePtr(), sizeof(result));
		RetrieveInt64();
		return socketops::NetworkToHost64(result);
	}

	int32_t ReadInt32()
	{
		int32_t result = 0;
		memcpy(&result, GetReadablePtr(), sizeof(result));
		RetrieveInt32();
		return socketops::NetworkToHost32(result);
	}

	int16_t ReadInt16()
	{
		int16_t result = 0;
		memcpy(&result, GetReadablePtr(), sizeof(result));
		RetrieveInt16();
		return socketops::NetworkToHost16(result);
	}

	int8_t ReadInt8()
	{
		int8_t result = *GetReadablePtr();
		RetrieveInt8();
		return result;
	}

	void Prepend(const void* data, size_t len)
	{
		read_index_ -= len;
		const char* d = static_cast<const char*>(data);
		std::copy(d, d + len, GetBegin() + read_index_);
	}

	void PrependInt64(int64_t x)
	{
		int64_t be64 = socketops::HostToNetwork64(x);
		Prepend(&be64, sizeof(be64));
	}

	void PrependInt32(int32_t x)
	{
		int32_t be32 = socketops::HostToNetwork32(x);
		Prepend(&be32, sizeof(be32));
	}

	void PrependInt16(int16_t x)
	{
		int16_t be16 = socketops::HostToNetwork16(x);
		Prepend(&be16, sizeof(be16));
	}

	void PrependInt8(int8_t x)
	{
		Prepend(&x, sizeof(x));
	}

	void Shrink(size_t reserve)
	{
		Buffer other;
		other.EnsureWritable(GetReadableSize() + reserve);
		other.Append(GetReadablePtr(), GetReadableSize());
		Swap(other);
	}

	size_t GetCapacity() const
	{
		return buffer_.capacity();
	}

	ssize_t ReadFd(int fd, int* saved_errno);
private:
	char* GetBegin() { return &*buffer_.begin(); }
	const char* GetBegin() const { return &*buffer_.begin(); }

	void MakeSpace(size_t len)
	{
		if (GetWritableSize() + GetPrependableSize() < len + kPrepend)
		{
			buffer_.resize(write_index_ + len);
		}
		else
		{
			size_t readable = GetReadableSize();
			std::copy(GetBegin() + read_index_, GetBegin() + write_index_, GetBegin() + kPrepend);
			read_index_ = kPrepend;
			write_index_ = read_index_ + readable;
		}
	}

	std::vector<char> buffer_;
	size_t read_index_;
	size_t write_index_;

	static const char kCRLF[];
};

#endif // BUFFER_H