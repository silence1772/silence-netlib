#include "eventloop.h"
#include <poll.h>
#include <iostream>

EventLoop::EventLoop()
{
	std::cout << "EventLoop " << this << " created" << std::endl;
}

EventLoop::~EventLoop()
{
	std::cout << "EventLoop: Bye~~" << std::endl;
}

void EventLoop::Loop()
{
	std::cout << "Waitting poll......" << std::endl;
	poll(NULL, 0, 5 * 1000);
	std::cout << "Polling timeout and Nothing happend" << std::endl;
}

