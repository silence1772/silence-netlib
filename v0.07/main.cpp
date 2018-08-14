#include <sys/time.h>
#include <iostream>
#include <string>
#include "eventloop.h"
#include "timestamp.h"
#include "thread.h"
#include "log/logger.h"

int cnt = 0;
EventLoop* g_loop;

void run2()
{
	LOG_INFO << "log_info";
	LOG_TRACE << "log_trace";
	LOG_DEBUG << "log_debug";
}

void run1()
{
	LOG_INFO << "test";
	g_loop->RunEvery(2 * Timestamp::kMicroSecondsPerSecond, run2);
}

int main()
{
	EventLoop loop;
	g_loop = &loop;

	loop.RunAfter(2 * Timestamp::kMicroSecondsPerSecond, run1);
	loop.Loop();
}