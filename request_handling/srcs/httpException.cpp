#include "HttpException.hpp"

HttpException::HttpException(int code, const std::string& msg)
    : _statusCode(code), _message(msg) {}

HttpException::HttpException(const HttpException& other)
	: _statusCode(other._statusCode), _message(other._message) {}

HttpException& HttpException::operator=(const HttpException& other)
{
	if (*this != other)
	{
		_statusCode = other._statusCode;
		_message = other._message;
	}
	return *this;
}

HttpException::~HttpException() {}

const char* HttpException::what() const throw()
{
	return _message.c_str();
}

int HttpException::getStatusCode() const
{
	return _statusCode;
}
