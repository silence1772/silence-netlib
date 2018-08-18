#include <unistd.h>
#include <iostream>
#include "tcpserver.h"
#include "eventloop.h"
#include "inetaddress.h"
#include "buffer.h"

std::string message1;
std::string message2;

void OnConnection(const TcpConnectionPtr& conn)
{
	if (conn->IsConnected())
	{
		std::cout << "new connection " << conn->GetName() << " from " << conn->GetPeerAddr().ToHostPort() << std::endl;

		sleep(5);
		conn->Send(message1);
		conn->Send(message2);
		conn->Shutdown();
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
}

int main(int argc, char* argv[])
{
	int len1 = 100;
	int len2 = 200;
	if (argc > 2)
	{
		len1 = atoi(argv[1]);
		len2 = atoi(argv[2]);
	}
	message1.resize(len1);
	message2.resize(len2);
	std::fill(message1.begin(), message1.end(), 'A');
	std::fill(message2.begin(), message2.end(), 'B');

	InetAddress listen_addr(8888);
 	EventLoop loop;

 	TcpServer server(&loop, listen_addr);
 	server.SetConnectionCallback(OnConnection);
 	server.SetMessageCallback(OnMessage);
 	server.Start();

	loop.Loop();
}