#include <iostream>
#include "tcpserver.h"
#include "eventloop.h"
#include "inetaddress.h"

void OnConnection(const TcpConnectionPtr& conn)
{
	if (conn->IsConnected())
	{
		std::cout << "new connection " << conn->GetName() << " from " << conn->GetPeerAddr().ToHostPort() << std::endl;
	}
	else
	{
		std::cout << "connection " << conn->GetName() << " is down" << std::endl;
	}
}

void OnMessage(const TcpConnectionPtr& conn, const char* data, ssize_t len)
{
	std::cout << "received " << len << " bytes from connection " << conn->GetName() << std::endl;
}

int main()
{
	InetAddress listen_addr(8888);
 	EventLoop loop;

 	TcpServer server(&loop, listen_addr);
 	server.SetConnectionCallback(OnConnection);
 	server.SetMessageCallback(OnMessage);
 	server.Start();

	loop.Loop();
}