#include "IPAddress.h"
#include "SocektException.h"
#include <stdexcept>

IPAddress::IPAddress()
{
	WSAStartup(MAKEWORD(2, 2), &this->wsa);
}

IPAddress::IPAddress(const std::string & address): IPAddress(address.c_str())
{
}

IPAddress::IPAddress(const char * address) : IPAddress()
{
	auto hent2 = gethostbyname(address);
	if (hent2 == NULL) {
		throw std::runtime_error(std::string(address) +" is not a valid address");
	}
	hent = *hent2;
}

IPAddress::IPAddress(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3)
{
	IPAddress(byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24));
}

IPAddress::IPAddress(uint32_t address)
{
	WSAStartup(MAKEWORD(2, 2), &this->wsa);
	auto hent2 = gethostbyaddr((char*)address, sizeof(address), AF_INET);
	if (hent2 == nullptr)
	{
		throw std::runtime_error("invalid ip");
	}
	hent = *hent2;
}

std::string IPAddress::toString() const
{
	return std::string(hent.h_name);
}

uint32_t IPAddress::toInteger() const
{
	struct sockaddr_in addr;
	addr.sin_addr = *reinterpret_cast<struct in_addr *>(&hent.h_addr);
	uint32_t clientIpAddr = ntohl(addr.sin_addr.s_addr);
	return clientIpAddr;
}
IPAddress::~IPAddress()
{
}
