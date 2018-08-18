#include <unistd.h>
#include <iostream>
#include "tcpserver.h"
#include "eventloop.h"
#include "inetaddress.h"
#include "buffer.h"

void OnConnection(const TcpConnectionPtr& conn)
{
	if (conn->IsConnected())
	{
		std::cout << "new connection " << conn->GetName() << " from " << conn->GetPeerAddr().ToHostPort() << std::endl;
	}
	else
	{
		std::cout << "the connection " << conn->GetName() << " is close." << std::endl;
	}
}

void OnMessage(const TcpConnectionPtr& conn, Buffer* buf)
{
	std::cout << "received " << buf->GetReadableSize() << " bytes from connection " << conn->GetName() << std::endl;
	std::cout << buf->RetrieveAllAsString() << std::endl;

	conn->Send("Hi~\n");
}

int main(int argc, char* argv[])
{
	InetAddress listen_addr(8888);
 	EventLoop loop;

 	TcpServer server(&loop, listen_addr);
 	server.SetConnectionCallback(OnConnection);
 	server.SetMessageCallback(OnMessage);
 	if (argc > 1)
 	{
 		server.SetThreadNum(atoi(argv[1]));
 	}
 	server.Start();

	loop.Loop();
}