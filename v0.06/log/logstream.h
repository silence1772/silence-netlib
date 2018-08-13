#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include <string.h>
#include <string>

using std::string;

const int kSmallBufferSize = 4000;
const int kLargeBufferSize = 4000 * 1000;

template<int SIZE>
class FixedBuffer
{
public:
	FixedBuffer() : cur_(data_) {}

	~FixedBuffer() {}

	void Append(const char* /*restrict*/ buf, size_t len)
	{
		if (implicit_cast<size_t>(GetAvailbale()) > len)
		{
			memcpy(cur_, buf, len);
			cur_ += len;
		}
	}

	const char* GetData() const { return data_; }
	int GetLength() const { return static_cast<int>(cur_ - data_); }
	char* GetCur() { return cur_; }
	int GetAvailbale() const { return static_cast<int>(GetEnd() - cur_); }

	void Add(size_t len) { cur_ += len; }
	void Reset() { cur_ = data_; }
	void Bzero() { bzero(data_, sizeof(data_)); }
private:
	const char* GetEnd() const { return data_ + sizeof(data_); }

	char data_[SIZE];
	char* cur_;
}


class LogStream
{
public:
	typedef FixedBuffer<kSmallBufferSize> Buffer;

	LogStream& operator<<(bool);
	LogStream& operator<<(short);
	LogStream& operator<<(unsigned short);
	LogStream& operator<<(int);
	LogStream& operator<<(unsigned int);
	LogStream& operator<<(long);
	LogStream& operator<<(unsigned long);
	LogStream& operator<<(long long);
	LogStream& operator<<(unsigned long long);
	LogStream& operator<<(const void*);
	LogStream& operator<<(float);
	LogStream& operator<<(double);
	LogStream& operator<<(char);
	LogStream& operator<<(const char*);
	LogStream& operator<<(const unsigned char* str);
	LogStream& operator<<(const string&);

	void Append(const char* data, int len) { buffer_.Append(data, len); }
	const Buffer& GetBuffer() const { return buffer_; }
	void ResetBuffer() { buffer_.Reset(); }
private:
	template<typename T>
	void FormatInteger(T);

	Buffer buffer_;
	static const int kMaxNumericSize = 32;
};


#endif // LOGSTREAM_H