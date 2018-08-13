#ifndef LOGGER_H
#define LOGGER_H

#include "logstream.h"
#include "timestamp.h"

class Logger
{
public:
	enum LogLevel
	{
		TRACE,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL,
		NUM_LOG_LEVELS,
	};

	Logger(const char* file_name, int line);
	Logger(const char* file_name, int line, LogLevel level);
	Logger(const char* file_name, int line, LogLevel level, const char* func_name);
	Logger(const char* file_name, int line, bool is_abort);
	~Logger();

	LogStream& GetStream() { return stream_; }

	static LogLevel GetLogLevel();
	static void SetLogLevel(LogLevel level);

	typedef void (*OutputFunc)(const char* msg, int len);
	typedef void (*FlushFunc)();
	static void SetOutput(OutputFunc);
	static void SetFlush(FlushFunc);

private:
	Timestamp time_;
	LogStream stream_;
	LogLevel level_;
	int line_;
	const char* file_name_; 
};


extern Logger::LogLevel g_loglevel;

inline Logger::LogLevel Logger::GetLogLevel()
{
	return g_loglevel;
}

#define LOG_TRACE if (Logger::GetLogLevel() <= Logger::TRACE) Logger(__FILE__, __LINE__, Logger::TRACE, __func__).GetStream()
#define LOG_DEBUG if (Logger::GetLogLevel() <= Logger::DEBUG) Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).GetStream()
#define LOG_INFO if (Logger::GetLogLevel() <= Logger::INFO) Logger(__FILE__, __LINE__, Logger::INFO, __func__).GetStream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).GetStream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).GetStream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).GetStream()
#define LOG_SYSERR Logger(__FILE__, __LINE__, false).GetStream()
#define LOG_SYSFATAL Logger(__FILE__, __LINE__, true).GetStream()

#endif // LOGGER_H