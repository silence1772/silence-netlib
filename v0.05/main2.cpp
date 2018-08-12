#include <unistd.h>
#include <iostream>
#include "eventloop.h"
#include "eventloopthread.h"
#include "currentthread.h"


void test()
{
	std::cout << "test(): pid " << GetTid() << std::endl;
}

int main()
{
	std::cout << "main(): pid " << GetTid() << std::endl;
	EventLoopThread loop_thread;
	EventLoop* loop = loop_thread.StartLoop();
	loop->RunInLoop(test);
	sleep(1);
	loop->RunAfter(2, test);
	sleep(3);
	loop->Quit();
}