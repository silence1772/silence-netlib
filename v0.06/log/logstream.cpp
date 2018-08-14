#include "logstream.h"
#include <algorithm>
#include <string.h>
#include <stdint.h>

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

const char digitsHex[] = "0123456789ABCDEF";

// Efficient Integer to String Conversions, by Matthew Wilson.
template<typename T>
size_t Convert(char buf[], T value)
{
	T i = value;
	char* p = buf;

	do
	{
		int lsd = static_cast<int>(i % 10);
		i /= 10;
		*p++ = zero[lsd];
	} while (i != 0);

	if (value < 0)
	{
		*p++ = '-';
	}
	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}

size_t ConvertHex(char buf[], uintptr_t value)
{
	uintptr_t i = value;
	char* p = buf;

	do
	{
		int lsd = static_cast<int>(i % 16);
		i /= 16;
		*p++ = digitsHex[lsd];
	} while (i != 0);

	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}


template<typename T>
void LogStream::FormatInteger(T v)
{
	if (buffer_.GetAvailbale() >= kMaxNumericSize)
	{
		size_t len = Convert(buffer_.GetCur(), v);
		buffer_.Add(len);
	}
}

LogStream& LogStream::operator<<(bool v)
{
	buffer_.Append(v ? "1" : "0", 1);
	return *this;
}

LogStream& LogStream::operator<<(short v)
{
	*this << static_cast<int>(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
	*this << static_cast<unsigned int>(v);
	return *this;
}

LogStream& LogStream::operator<<(int v)
{
	FormatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
	FormatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long v)
{
	FormatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
	FormatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long long v)
{
	FormatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
	FormatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(const void* p)
{
	uintptr_t v = reinterpret_cast<uintptr_t>(p);
	if (buffer_.GetAvailbale() >= kMaxNumericSize)
	{
		char* buf = buffer_.GetCur();
		buf[0] = '0';
		buf[1] = 'x';
		size_t len = ConvertHex(buf + 2, v);
		buffer_.Add(len + 2);
	}
	return *this;
}

LogStream& LogStream::operator<<(float v)
{
	*this << static_cast<double>(v);
	return *this;
}

LogStream& LogStream::operator<<(double v)
{
	if (buffer_.GetAvailbale() >= kMaxNumericSize)
	{
		int len = snprintf(buffer_.GetCur(), kMaxNumericSize, "%.12g", v);
		buffer_.Add(len);
	}
	return *this;
}

LogStream& LogStream::operator<<(char v)
{
	buffer_.Append(&v, 1);
	return *this;
}

LogStream& LogStream::operator<<(const char* str)
{
	if (str)
	{
		buffer_.Append(str, strlen(str));
	}
	else
	{
		buffer_.Append("(null)", 6);
	}
	return *this;
}

LogStream& LogStream::operator<<(const unsigned char* str)
{
	return operator<<(reinterpret_cast<const char*>(str));
}

LogStream& LogStream::operator<<(const string& v)
{
	buffer_.Append(v.c_str(), v.size());
	return *this;
}