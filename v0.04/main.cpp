#include <sys/time.h>
#include <iostream>
#include <string>
#include "eventloop.h"
#include "timestamp.h"
#include "thread.h"

int cnt = 0;
EventLoop* g_loop;

void print()
{
	std::cout << "Do RunAfter() at " << Timestamp::GetNow().ToFormattedString() << std::endl;
}

void print2()
{
	std::cout << "Do RunEvery() at " << Timestamp::GetNow().ToFormattedString() << std::endl;
	if (++cnt == 5) g_loop->Quit();
}

void threadfunc()
{
	EventLoop loop;
	g_loop = &loop;

	g_loop->RunAfter(5 * Timestamp::kMicroSecondsPerSecond, print);
	g_loop->RunEvery(2 * Timestamp::kMicroSecondsPerSecond, print2);

	g_loop->Loop();
}

int main()
{
	// EventLoop loop;
	// g_loop = &loop;

	Thread t(threadfunc);
	t.Start();
	t.Join();
}