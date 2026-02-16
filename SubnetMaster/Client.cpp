#include "Client.hpp"

Client::Client() : _fd(-1){
    _lastActivity = time(NULL);
}

Client::Client(int fd): _fd(fd){
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


bool    Client::isRequestComplete(){
    if (!hasHeadersSeparator())
        return false;
    
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

Client::Client(const Client& src) : _requestBuffer(src._requestBuffer), _responseBuffer(src._responseBuffer), _lastActivity(src._lastActivity), _fd(src._fd){}

Client& Client::operator=(const Client& rhs){
    if (this != &rhs){
        _fd = rhs._fd;
        _lastActivity = rhs._lastActivity;
        _requestBuffer = rhs._requestBuffer;
        _responseBuffer = rhs._responseBuffer;
    }
    return *this;
}