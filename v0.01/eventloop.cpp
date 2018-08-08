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
	std::cout << "Polling timeout" << std::endl;
	std::cout << "EventLoop " << this << " stop looping" << std::endl;
}

