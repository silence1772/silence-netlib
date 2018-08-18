#include <unistd.h>
#include <iostream>
#include "tcpserver.h"
#include "eventloop.h"
#include "inetaddress.h"
#include "buffer.h"

std::string message;

void OnConnection(const TcpConnectionPtr& conn)
{
	if (conn->IsConnected())
	{
		std::cout << "new connection " << conn->GetName() << " from " << conn->GetPeerAddr().ToHostPort() << std::endl;

		conn->Send(message);

	}
	else
	{
		std::cout << "the connection " << conn->GetName() << " is close." << std::endl;
	}
}

void OnWriteComplete(const TcpConnectionPtr& conn)
{
	conn->Send(message);
}

void OnMessage(const TcpConnectionPtr& conn, Buffer* buf)
{
	std::cout << "received " << buf->GetReadableSize() << " bytes from connection " << conn->GetName() << std::endl;
	std::cout << buf->RetrieveAllAsString() << std::endl;
}

int main(int argc, char* argv[])
{
	std::string line;
	for (int i = 33; i < 127; ++i)
	{
		line.push_back(char(i));
	}
	line += line;

	for (size_t i = 0; i < 127-33; ++i)
	{
		message += line.substr(i, 72) + '\n';
	}

	InetAddress listen_addr(8888);
 	EventLoop loop;

 	TcpServer server(&loop, listen_addr);
 	server.SetConnectionCallback(OnConnection);
 	server.SetMessageCallback(OnMessage);
 	server.SetWriteCompleteCallback(OnWriteComplete);
 	server.Start();

	loop.Loop();
}