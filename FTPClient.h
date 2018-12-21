#pragma once
#include <vector>
#include <list>
#include <functional>
#include <memory>
#include "SocketTCP.h"

class FTPClient
{
public:
	std::string Connect(std::string addr, uint16_t port);
	void Login(std::string login, std::string password);
	std::string Quit();
	std::vector<std::string> RetrLines(std::string cmd);
	std::string SendCommand(std::string cmd);
	std::vector<std::string> Nlst(std::string path);
	std::string Pwd();
	std::string Cwd(std::string path);
	void ExploreRecursively(std::string path, std::list<std::string> &container, int depth = 0);
    std::vector<std::string> Split(std::string& s, std::string tok = "\r\n") const;
	std::string Cdup();
private:
	bool isConnected = false;
	bool isDataChannelOpen = false;
	SocketTCP cmdSock{};
	std::string ReadFTPMultilineResponse() const;
	auto GetDataStreamIPAndPortFromResponse(std::string) const;
	void CheckForErrors(std::string) const;
	std::unique_ptr<SocketTCP> OpenDataChannel();
};

