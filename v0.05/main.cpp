#include <sys/time.h>
#include <iostream>
#include <string>
#include "eventloop.h"
#include "timestamp.h"
#include "thread.h"

int cnt = 0;
EventLoop* g_loop;

void run4()
{
	std::cout << "run4()" << std::endl;
	g_loop->Quit();
}

void run3()
{
	std::cout << "run3()" << std::endl;
}

void run2()
{
	std::cout << "run2()" << std::endl;
	g_loop->QueueInLoop(run3);
	g_loop->QueueInLoop(run4);
}

void run1()
{
	std::cout << "run1()" << std::endl;
	g_loop->RunInLoop(run2);
}

int main()
{
	EventLoop loop;
	g_loop = &loop;

	loop.RunAfter(2 * Timestamp::kMicroSecondsPerSecond, run1);

	loop.Loop();
}