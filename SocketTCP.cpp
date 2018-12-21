#include "SocektException.h"
#include "SocketTCP.h"
#include "IPAddress.h"
#include <sstream>
SocketTCP::SocketTCP(): sock(-1)
{
	WSAStartup(MAKEWORD(2, 2), &this->wsa);
}

SocketTCP::State SocketTCP::TCPConnect(const IPAddress & remoteAddress, unsigned short remotePort, int timeout)
{
	std::stringstream s;
	s << remotePort;
	struct addrinfo hints;
	struct addrinfo *result, *resultPointer;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	const int getaddrinfoReturnCode =
		getaddrinfo(remoteAddress.toString().c_str(), s.str().c_str(), &hints, &result);
	if (getaddrinfoReturnCode != 0)
	{


        std::wstring ws(gai_strerror(getaddrinfoReturnCode));
        std::string test( ws.begin(), ws.end() );
        throw std::runtime_error(test.c_str());
	}

	for (resultPointer = result; resultPointer != nullptr; resultPointer = resultPointer->ai_next)
	{
		sock = socket(resultPointer->ai_family, resultPointer->ai_socktype, resultPointer->ai_protocol);
		if (sock == -1)
		{
			continue;
		}

		if (connect(sock, resultPointer->ai_addr, resultPointer->ai_addrlen) != -1)
		{
			break;
		}

		closesocket(sock);
	}

	if (resultPointer == NULL)
	{
		throw std::runtime_error("Cannot establish connection to the server");
	}
	::freeaddrinfo(result);
	return state;
}

SocketTCP::State SocketTCP::TCPSend(const char *data, std::size_t size) const
{
	int total = 0;
	int bytesleft = size; 

	while (total < size) {
		int n = send(sock, data + total, bytesleft, 0);
		if (n == -1) { throw SocektException{"Error occurred during send"}; }
		total += n;
		bytesleft -= n;
	}

	return State::Done;
}

SocketTCP::State SocketTCP::TCPSendString(std::string s) const
{
	auto sent = 0;
	while(s.length()>0)
	{
		sent = send(sock, static_cast<const char*>(s.c_str()), s.length(), 0);
		if (sent<0)
		{
			throw SocektException("Error while sending data to the remote host");
		}
		s = s.substr(0, s.length()-sent);
	}
	return State::Done;
}

SocketTCP::State SocketTCP::TCPReceive(void * data, std::size_t size, std::size_t & received) const
{
	received = recv(sock, (char*)data, size, 0);
	return State::Done;
}

SocketTCP::State SocketTCP::TCPReceiveChar(char* c) const
{
	if (read(c, 1) > 0)
	{
		return State::Done;
	}
	return State::NotReady;
}

void SocketTCP::ReadAll(std::string& s) const
{
	char buff;
	s.clear();
	while(TCPReceiveChar(&buff) == State::Done)
	{
		s += buff;
	}
}

size_t SocketTCP::read(char* buffer, size_t size) const
{
	int64_t bytesRead = 0;
	if (!isReadyToRead())
	{
		return bytesRead;
	}

	bytesRead = ::recv(sock, buffer, size,0);
	if (bytesRead < 0)
	{
		throw std::runtime_error("Unable to resolve data from remote host");
	}

	return bytesRead;
}

bool SocketTCP::isReadyToRead() const
{
	fd_set recieveFd;
	struct timeval timeout;

	FD_ZERO(&recieveFd);
	FD_SET(sock, &recieveFd);

	/* 30 seconds timeout */
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	const int selectReturnValue = select(sock + 1, &recieveFd, NULL, NULL, &timeout);
	return selectReturnValue>0? true : false;

}
size_t SocketTCP::TCPReceiveLine(std::string& line) const
{

	char buffer[2] = "";
	size_t bytesRead = 0;
	line.clear();
	if(TCPReceiveChar(buffer+1) == State::Done)
	{
		do
		{
			line.push_back(buffer[0]);
			buffer[0] = buffer[1];
			++bytesRead;

		} while (TCPReceiveChar(buffer + 1) == State::Done &&
			buffer[0] != '\r' && buffer[1] != '\n');
		line.erase(0, 1);
	}
	return bytesRead;
}

SocketTCP::~SocketTCP()
{
	closesocket(sock);
	state = State::Disconnected;
}
