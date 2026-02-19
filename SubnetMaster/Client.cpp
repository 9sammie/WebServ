#include "Client.hpp"
#include "Colors.hpp"
#include <iostream>

Client::Client() : _fd(-1), _closeAfterResponse(false){
    _lastActivity = time(NULL);
}

Client::Client(int fd): _fd(fd), _closeAfterResponse(false){
    _lastActivity = time(NULL);
}

void Client::store(const std::string& content, BufferType type){
    if (type == REQUEST)
        _requestBuffer += content;
    if (type == RESPONSE)
        _responseBuffer += content;
}

void Client::clean(BufferType type){
    if (type == REQUEST)
        _requestBuffer.clear();
    if (type == RESPONSE)
        _responseBuffer.clear();
}

bool    Client::hasHeadersSeparator(){
    if (_requestBuffer.find("\r\n\r\n") != std::string::npos)
        return true;
    return false;
}

bool Client::hasContentLengthHeader(){
    std::string headerRequest = _requestBuffer.substr(0, _requestBuffer.find("\r\n\r\n"));
    for (size_t i = 0; i < headerRequest.size(); ++i)
        headerRequest[i] = std::tolower((unsigned char)headerRequest[i]);
    if (headerRequest.find("content-length:") != std::string::npos)
        return true;
    return false;
}

bool    Client::hasBody(){
    size_t bodyStart = _requestBuffer.find("\r\n\r\n") + 4;
    if (_requestBuffer.size() > bodyStart)
        return true;
    return false;
}

size_t Client::actualBodySize(){
    size_t bodyStart = _requestBuffer.find("\r\n\r\n") + 4;
    std::string body = _requestBuffer.substr(bodyStart);
    return body.size();
}

ssize_t      Client::getContentLenthSize(){

    std::string headerRequest = _requestBuffer.substr(0, _requestBuffer.find("\r\n\r\n"));
    for (size_t i = 0; i < headerRequest.size(); ++i)
        headerRequest[i] = std::tolower((unsigned char)headerRequest[i]);
    size_t contentLengthLineStart = headerRequest.find("content-length:") +15;
    size_t contentLengthLineEnd = headerRequest.find("\r\n", contentLengthLineStart);
    std::string contentLengthLine = headerRequest.substr(contentLengthLineStart, contentLengthLineEnd - contentLengthLineStart);
    
    char *endptr;
    const char *start = contentLengthLine.c_str();
    errno = 0;
    long bodySize = std::strtol(start, &endptr, 10);
    if (endptr == start || errno == ERANGE)
        return -1;
    while (*endptr == ' ' || *endptr == '\t')
        endptr++;
    if (*endptr != '\0' || bodySize < 0)
        return -1;
    return bodySize;
}

bool    Client::isRequestComplete(){
    if (!hasHeadersSeparator())
        return false;
    if (!hasBody()){
        std::cout << BRIGHT_BROWN << "DEBUG: no body !" << RESET << std::endl;
        if (!hasContentLengthHeader())
        return true;
    }
    if (!hasContentLengthHeader()){
        std::cout << BRIGHT_BROWN << "DEBUG: no body !" << RESET << std::endl;
        _closeAfterResponse = true;
        return true;
    }
    ssize_t bodySize = getContentLenthSize();
    if (bodySize < 0){
        _closeAfterResponse = true;
        return true;
    }
    else{
        if ((size_t)bodySize > actualBodySize())
            return false;
    }
    return true;
}

int Client::getFd() const{
    return _fd;
}

const std::string& Client::getBuffer(BufferType type)const{
    if (type == REQUEST)
        return _requestBuffer;
    return _responseBuffer;
}

time_t Client::timeSinceLastActivity(){
    return time(NULL) - _lastActivity;
}

void    Client::updateActivity(){
    _lastActivity = time(NULL);
}

Client::~Client(){}

Client::Client(const Client& src) : _requestBuffer(src._requestBuffer), _responseBuffer(src._responseBuffer),
_lastActivity(src._lastActivity), _fd(src._fd), _closeAfterResponse(src._closeAfterResponse){}

Client& Client::operator=(const Client& rhs){
    if (this != &rhs){
        _fd = rhs._fd;
        _lastActivity = rhs._lastActivity;
        _requestBuffer = rhs._requestBuffer;
        _responseBuffer = rhs._responseBuffer;
        _closeAfterResponse = rhs._closeAfterResponse;
    }
    return *this;
}