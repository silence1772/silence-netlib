#include <string.h> // for bzero()
#include <unistd.h> // for close()
#include <sys/timerfd.h>
#include <iostream>
#include "eventloop.h"
#include "eventitem.h"

// global varible
EventLoop* g_loop;

void timeout()
{
	std::cout << "==============================" << std::endl;
	std::cout << "user function callback in here" << std::endl;
	std::cout << "this will quit the EventLoop" << std::endl;
	std::cout << "==============================" << std::endl;
	g_loop->Quit();
}

int main()
{
	EventLoop loop;
	g_loop = &loop;
	
	int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  	EventItem eventitem(&loop, timerfd);
  	eventitem.SetReadCallback(timeout);
  	eventitem.EnableReading();

  	struct itimerspec howlong;
  	bzero(&howlong, sizeof howlong);
  	howlong.it_value.tv_sec = 8;
  	timerfd_settime(timerfd, 0, &howlong, NULL);

  	loop.Loop();

	close(timerfd);
	return 0;
}
