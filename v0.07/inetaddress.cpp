#include "inetaddress.h"
#include <string.h> //bzero
#include <netinet/in.h>
#include "socketops.h"

InetAddress::InetAddress(uint16_t port)
{
	bzero(&addr_, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_addr.s_addr = HostToNetwork32(INADDR_ANY);
	addr_.sin_port = HostToNetwork16(port);
}

InetAddress::InetAddress(const std::string &ip, uint16_t port)
{
	bzero(&addr_, sizeof(addr_));
	FromHostPort(ip.c_str(), port, &addr_);
}

std::string InetAddress::ToHostPort() const
{
	char buf[32];
	ToHostPort(buf, sizeof(buf), addr_);
	return buf;
}