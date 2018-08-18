#include <iostream>
#include "connector.h"
#include "eventloop.h"

EventLoop* g_loop;

void ConnectCallback(int sockfd)
{
	std::cout << "connected." << std::endl;
	g_loop->Quit();
}

int main(int argc, char* argv[])
{
 	EventLoop loop;
 	g_loop = &loop;

 	InetAddress server_addr("127.0.0.1", 8888);
 	std::unique_ptr<Connector> connector(new Connector(&loop, server_addr));
 	connector->SetNewConnectionCallback(ConnectCallback);
 	connector->Start();

	loop.Loop();
}