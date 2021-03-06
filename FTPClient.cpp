#include "FTPClient.h"
#include <regex>
#include <chrono>
#include <algorithm>
#include <thread>
#include "transientexception.h"
#define FOLDER_SEP "/"
using namespace std::chrono_literals;

std::string FTPClient::Connect(std::string addr, uint16_t port)
{
    cmdSock.TCPConnect(addr,port);
	isConnected = true;
	return ReadFTPMultilineResponse();
}

void FTPClient::Login(std::string login, std::string password)
{
	cmdSock.TCPSendString(std::string("USER ") + login + "\r\n");
	CheckForErrors(ReadFTPMultilineResponse());
	cmdSock.TCPSendString(std::string("PASS ") + password + "\r\n");
	CheckForErrors(ReadFTPMultilineResponse());
	OpenDataChannel();
}

std::string FTPClient::Quit()
{
	auto response = SendCommand("QUIT");
	return response;
}

std::string FTPClient::SendCommand(std::string cmd)
{
	cmdSock.TCPSendString(cmd+"\r\n");
	auto response = ReadFTPMultilineResponse();
	CheckForErrors(response);
	return response;
}

std::vector<std::string> FTPClient::Nlst(std::string path = "")
{
	std::vector<std::string> data;
	std::string buffer;
	auto dataSock = OpenDataChannel();
	SendCommand("NLST " + path);
    dataSock->TCPReveiveUtilClosed(buffer);
	ReadFTPMultilineResponse();
	data = Split(buffer);
	return data;
}

std::string FTPClient::Pwd()
{
	auto response = SendCommand("PWD");
	return response;
}

std::string FTPClient::Cwd(std::string path)
{
	auto response = SendCommand("CWD " + path);
	return response;
}

void FTPClient::ExploreRecursively(std::string path,std::list<std::string> &container, int depth)
{
	Cwd(path);
	auto fileList = Nlst(path);
    std::string prefix{ "\t",static_cast<size_t>(depth) };
	for(auto file : fileList)
	{
		auto temp = prefix + file;
        auto split = Split(temp,FOLDER_SEP);
        container.push_back(prefix+FOLDER_SEP+split.back());
		try
		{
			ExploreRecursively(file,container, depth+1);
            std::this_thread::sleep_for(10ms);
		}
        catch(TransientException &)
		{
			//do nothing as it is not directory
		}
	}
}

std::string FTPClient::Cdup()
{
	return SendCommand("CDUP");
}

std::string FTPClient::ReadFTPMultilineResponse() const
{
	std::string s;
	std::string buff;
    cmdSock.TCPReceiveUntil(s);
	if (s.substr(3, 1) == "-")
	{
		const auto endCode = s.substr(0, 3) + " ";
		while (true)
		{
            cmdSock.TCPReceiveUntil(buff);
			s += "\r\n" + buff;
			if (buff.substr(0, 4) == endCode)
				break;
		}
	}
	return s;
}

auto FTPClient::GetDataStreamIPAndPortFromResponse(std::string response) const
{
	std::regex rx{ R"(\(.*\))" };
	std::smatch results;
	std::vector<std::string> numbers;
	if (std::regex_search(response, results, rx))
	{
		std::string d = results[0];
		std::regex digit{ R"(\d+)" };
		auto digits_begin =
			std::sregex_iterator(d.begin(), d.end(), digit);
		auto digits_end = std::sregex_iterator();
		for (auto i = digits_begin; i != digits_end; i++)
		{
			std::smatch match = *i;
			std::string s = match.str();
			numbers.push_back(s);
		}
	}
	else
	{
        throw std::runtime_error("Server did not open data stream, message: "+ response);
	}
	auto port = atoi(numbers[4].c_str()) * 256;
	port += atoi(numbers[5].c_str());
	std::string ip{};
	for (size_t i = 0; i < 4; ++i) {
		ip += numbers[i];
		ip += ".";
	}
	ip.pop_back();
	return std::make_pair(ip, port);
}

std::unique_ptr<SocketTCP> FTPClient::OpenDataChannel()
{
	std::unique_ptr<SocketTCP> dataSock{ new SocketTCP{} };
	auto response = SendCommand("PASV");
	auto [ip, port] = GetDataStreamIPAndPortFromResponse(response);
    dataSock->TCPConnect(ip, static_cast<std::uint16_t>(port));
	return dataSock;
}

std::vector<std::string> FTPClient::Split(std::string& s, std::string tok ) const
{
	std::vector<std::string> strs{};
		size_t pos = s.find(tok);
		size_t initialPos = 0;

		while (pos != std::string::npos) {
			strs.push_back(s.substr(initialPos, pos - initialPos));
			initialPos = pos + tok.length();

			pos = s.find(tok, initialPos);
		}
		if(initialPos != s.length())
            strs.push_back(s.substr(initialPos, std::min(pos, s.length()) - initialPos + tok.length()));

		return strs;
}

void FTPClient::CheckForErrors(std::string response) const
{
    int code = atoi(response.substr(0, 1).c_str());
	switch(code)
	{
    case 4:
        throw TransientException(response);
    case 5:
        throw PermamentException(response);
	default: ;
	}
}
