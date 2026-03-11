#ifndef HTTPEXCEPTION_HPP
#define HTTPEXCEPTION_HPP

#include <string>

class HttpException : public std::exception
{
private:
    int         _statusCode;
    std::string _message;
public:
    HttpException(int code, const std::string& msg);
	HttpException(const HttpException& other);
	HttpException& operator=(const HttpException& other);
	~HttpException();
    
    const char* what() const throw() { return _message.c_str(); }
    int getStatusCode() const { return _statusCode; }
};

#endif