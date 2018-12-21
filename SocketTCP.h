#pragma once
#include <WinSock2.h>
#include <string>
class IPAddress;

class SocketTCP
{
public:
	enum class State {
		Done, NotReady, Partial, Disconnected, Error
	};
	SocketTCP();
	SocketTCP::State TCPConnect(const IPAddress &remoteAddress, unsigned short remotePort, int timeout = 0);
	SocketTCP::State TCPSend(const char* data, size_t size) const;
	SocketTCP::State TCPSendString(std::string s) const;
	SocketTCP::State TCPReceive(void *data, size_t size, size_t &received) const;
	SocketTCP::State TCPReceiveChar(char *c) const;
	void ReadAll(std::string& s) const;
	size_t read(char * buffer, size_t size) const;
	bool isReadyToRead() const;
	size_t TCPReceiveLine(std::string &line) const;
	SocketTCP(SocketTCP &socket) = delete;
	~SocketTCP();
private:
	int sock;
	WSADATA wsa;
	State state = State::NotReady;
};