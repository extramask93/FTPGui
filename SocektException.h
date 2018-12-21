#pragma once
#include <stdexcept>
#include <string>
class SocektException : public std::runtime_error
{
public:
    SocektException(const char * message) : std::runtime_error(message) {}
};

