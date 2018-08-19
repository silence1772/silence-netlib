#include <unistd.h>
#include <iostream>
#include "tcpclient.h"
#include "eventloop.h"
#include "inetaddress.h"
#include "buffer.h"

void OnConnection(const TcpConnectionPtr& conn)
{
	if (conn->IsConnected())
	{
		std::cout << "new connection " << conn->GetName() << " from " << conn->GetPeerAddr().ToHostPort() << std::endl;
		conn->Send("Hello~\n");
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
 	EventLoop loop;
 	InetAddress server_addr("127.0.0.1", 8888);
 	TcpClient client(&loop, server_addr);

 	client.SetConnectionCallback(OnConnection);
 	client.SetMessageCallback(OnMessage);
 	client.EnableRetry();
 	client.Connect();

	loop.Loop();
}