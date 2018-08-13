#include "logger.h"
#include "currentthread.h"
#include "timestamp.h"

Logger::LogLevel InitLogLevel()
{
	if (getenv("LOG_TRACE"))
		return Logger::TRACE;
	else if (getenv("LOG_DEBUG"))
		return Logger::DEBUG;
	else
		return Logger::INFO;
}
Logger::LogLevel g_loglevel = InitLogLevel();


void DefaultOutput(const char* msg, int len)
{
  size_t n = fwrite(msg, 1, len, stdout);
}

void DefaultFlush()
{
  fflush(stdout);
}

Logger::OutputFunc g_output = DefaultOutput;
Logger::FlushFunc g_flush = DefaultFlush;


Logger::Logger(const char* file_name, int line)
{
	stream_ << Timestamp::GetNow().ToFormattedString() << ' '  << GetTid() << ' ' << INFO << ' ';
}

Logger::Logger(const char* file_name, int line, LogLevel level)
{
	stream_ << Timestamp::GetNow().ToFormattedString() << ' '  << GetTid() << ' ' << level << ' ';
}

Logger::Logger(const char* file_name, int line, LogLevel level, const char* func_name)
{
	stream_ << Timestamp::GetNow().ToFormattedString() << ' '  << GetTid() << ' ' << level << ' ' << func_name << ' ';
}

Logger::Logger(const char* file_name, int line, bool is_abort)
{
	stream_ << Timestamp::GetNow().ToFormattedString() << ' '  << GetTid() << ' ' << (is_abort?FATAL:ERROR) << ' ' << ' ';
}

Logger::~Logger()
{
	const LogStream::Buffer& buf(GetStream().GetBuffer());
	g_output(buf.GetData(), buf.GetLength());
	if (level_ == FATAL)
	{
		g_flush();
		abort();
	}
}

void Logger::SetLogLevel(Logger::LogLevel level)
{
	g_logLevel = level;
}

void Logger::SetOutput(OutputFunc out)
{
	g_output = out;
}

void Logger::SetFlush(FlushFunc flush)
{
	g_flush = flush;
}