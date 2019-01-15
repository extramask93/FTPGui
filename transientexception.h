#ifndef TRANSIENTEXCEPTION_H
#define TRANSIENTEXCEPTION_H
#include <string>
#include <stdexcept>
class TransientException : public std::runtime_error
{
public:
    TransientException(std::string message) : std::runtime_error(message) {}

};
class PermamentException : public TransientException
{
public:
    PermamentException(std::string message) :TransientException(message) {}

};

#endif // TRANSIENTEXCEPTION_H
