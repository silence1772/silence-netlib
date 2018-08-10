#include <sys/time.h>
#include <iostream>
#include <string>
#include "eventloop.h"
#include "timestamp.h"

int cnt = 0;
EventLoop* g_loop;

void print()
{
	std::cout << "Do RunAfter() at " << Timestamp::GetNow().ToFormattedString() << std::endl;
}

void print2()
{
	std::cout << "Do RunEvery() at " << Timestamp::GetNow().ToFormattedString() << std::endl;
	if (++cnt == 10) g_loop->Quit();
}

int main()
{
	EventLoop loop;
	g_loop = &loop;

	loop.RunAfter(5 * Timestamp::kMicroSecondsPerSecond, print);
	loop.RunEvery(2 * Timestamp::kMicroSecondsPerSecond, print2);

	loop.Loop();
}