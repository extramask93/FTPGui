#ifndef FTPEXCEPTION_H
#define FTPEXCEPTION_H
#include <stdexcept>

class FTPException : public std::runtime_error
{
public:
    FTPException(const std::string &message) : std::runtime_error(message.c_str()) {}
};

#endif // FTPEXCEPTION_H
