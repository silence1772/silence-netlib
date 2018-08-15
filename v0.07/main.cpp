#include <unistd.h>
#include <iostream>
#include "acceptor.h"
#include "eventloop.h"
#include "socketops.h"
#include "inetaddress.h"

void test(int sockfd, const InetAddress& peer_addr)
{
	std::cout << "Accept a new connection from " << peer_addr.ToHostPort() << std::endl;
	write(sockfd, "How are you?\n", 13);
	socketops::Close(sockfd);
}

int main()
{
	InetAddress listen_addr(8888);
 	EventLoop loop;

 	Acceptor acceptor(&loop, listen_addr);
 	acceptor.SetNewConnectionCallback(test);
 	acceptor.Listen();

	loop.Loop();
}