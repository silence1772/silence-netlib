#include <sys/time.h>
#include <iostream>
#include <string>
#include "eventloop.h"
#include "timestamp.h"
#include "thread.h"
#include "logger.h"

int cnt = 0;
EventLoop* g_loop;

void run1()
{
	LOG_INFO << "Test";
}

int main()
{
	EventLoop loop;
	g_loop = &loop;

	loop.RunAfter(2 * Timestamp::kMicroSecondsPerSecond, run1);

	loop.Loop();
}