#include "logger.h"
#include "string.h"
#include <errno.h>
#include "logstream.h"
#include "../currentthread.h"
#include "../timestamp.h"

__thread char t_errnobuf[512];
const char* strerror_tl(int saved_errno)
{
	return strerror_r(saved_errno, t_errnobuf, sizeof(t_errnobuf));
}

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

const char* g_loglevel_name[Logger::NUM_LOG_LEVELS] =
{
	"TRACE ",
	"DEBUG ",
	"INFO  ",
	"WARN  ",
	"ERROR ",
	"FATAL ",
};

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


Logger::Logger(const char* file_name, int line) : file_name_(file_name), line_(line), level_(INFO)
{
	stream_ << Timestamp::GetNow().ToFormattedString() << ' '  << GetTid() << ' ' << g_loglevel_name[level_];
}

Logger::Logger(const char* file_name, int line, LogLevel level) : file_name_(file_name), line_(line), level_(level)
{
	stream_ << Timestamp::GetNow().ToFormattedString() << ' '  << GetTid() << ' ' << g_loglevel_name[level_];
}

Logger::Logger(const char* file_name, int line, LogLevel level, const char* func_name) : file_name_(file_name), line_(line), level_(level)
{
	stream_ << Timestamp::GetNow().ToFormattedString() << ' '  << GetTid() << ' ' << g_loglevel_name[level_] << func_name << "():";
}

Logger::Logger(const char* file_name, int line, bool is_abort) : file_name_(file_name), line_(line), level_(is_abort?FATAL:ERROR)
{
	stream_ << Timestamp::GetNow().ToFormattedString() << ' '  << GetTid() << ' ' << g_loglevel_name[level_];
	if (errno != 0)
	{
		stream_ << strerror_tl(errno) << " (errno=" << errno << ") ";
	}
}

Logger::~Logger()
{
	stream_ << " - " << file_name_ << ':' << line_ << '\n';

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
	g_loglevel = level;
}

void Logger::SetOutput(OutputFunc out)
{
	g_output = out;
}

void Logger::SetFlush(FlushFunc flush)
{
	g_flush = flush;
}