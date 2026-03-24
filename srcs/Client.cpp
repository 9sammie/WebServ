#include "Client.hpp"
#include "Colors.hpp"
#include <iostream>
#include <cerrno>
#include <cstdlib>

Client::Client() : _fd(-1), _port(-1), _closeAfterResponse(false), _responseOffsetSent(0), _requestSize(0){
    _lastActivity = time(NULL);
    _cgiInfo.isCgi = false;
    _cgiInfo.pid = -1;
    _cgiInfo.pipeRead = -1;
    _cgiInfo.pipeWrite = -1;
    _cgiInfo.start_time = 0;
    _cgiInfo.bodyWrittenBytes = 0;
}

Client::Client(int fd, int port): _fd(fd),_port(port), _closeAfterResponse(false), _responseOffsetSent(0), _requestSize(0){
    _lastActivity = time(NULL);
    _cgiInfo.isCgi = false;
    _cgiInfo.pid = -1;
    _cgiInfo.pipeRead = -1;
    _cgiInfo.pipeWrite = -1;
    _cgiInfo.start_time = 0;
    _cgiInfo.bodyWrittenBytes = 0;
}

void Client::store(const std::string& content, BufferType type){
    if (type == REQUEST)
        _requestBuffer += content;
    if (type == RESPONSE)
        _responseBuffer += content;
    if (type == RAW)
        _rawBuffer += content;
}

void Client::clean(BufferType type){
    if (type == REQUEST)
        _requestBuffer.clear();
    if (type == RESPONSE)
        _responseBuffer.clear();
    if (type == RAW)
        _rawBuffer.clear();
}

bool    Client::hasHeadersSeparator(){
    if (_rawBuffer.find("\r\n\r\n") != std::string::npos)
        return true;
    return false;
}

bool Client::hasContentLengthHeader()const{
    std::string headerRequest = _rawBuffer.substr(0, _rawBuffer.find("\r\n\r\n"));
    for (size_t i = 0; i < headerRequest.size(); ++i)
        headerRequest[i] = std::tolower((unsigned char)headerRequest[i]);
    if (headerRequest.find("content-length:") != std::string::npos)
        return true;
    return false;
}

bool    Client::hasBody(){
    size_t bodyStart = _rawBuffer.find("\r\n\r\n") + 4;
    if (_rawBuffer.size() > bodyStart)
        return true;
    return false;
}

size_t Client::availableDataAfterHeaders(){
    size_t bodyStart = _rawBuffer.find("\r\n\r\n") + 4;
    std::string body = _rawBuffer.substr(bodyStart);
    return body.size();
}

ssize_t      Client::getContentLenthSize()const{

    std::string headerRequest = _rawBuffer.substr(0, _rawBuffer.find("\r\n\r\n"));
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

//Add a check transfer-encoding function to check for chunked data
bool Client::isRequestComplete(){
    if (!hasHeadersSeparator())
        return false;
    if (!hasContentLengthHeader()){
        // if (hasBody())
        //     _closeAfterResponse = true;
        return true;
    }
    ssize_t bodySize = getContentLenthSize();
    if (bodySize < 0){
        _closeAfterResponse = true;
        return true;
    }
    return availableDataAfterHeaders() >= (size_t)bodySize;
}

int Client::getFd() const{
    return _fd;
}

const std::string& Client::getBuffer(BufferType type){//Could be better
    if (type == REQUEST)
        return _requestBuffer;
    else if (type == RESPONSE)
        return _responseBuffer;
    else
        return _rawBuffer;
}

Client::CgiInfo&    Client::getCgiInfo(){
    return _cgiInfo;
}


time_t Client::timeSinceLastActivity(){
    return time(NULL) - _lastActivity;
}

void    Client::updateActivity(){
    _lastActivity = time(NULL);
}

void        Client::resetCgiInfos(){
    _lastActivity = time(NULL);
    _cgiInfo.isCgi = false;
    _cgiInfo.pid = -1;
    _cgiInfo.pipeRead = -1;
    _cgiInfo.pipeWrite = -1;
    _cgiInfo.start_time = 0;
    _cgiInfo.bodyWrittenBytes = 0;
}

size_t&      Client::getResponseOffsetSent(){
    return _responseOffsetSent;
}

void        Client::resetResponseOffsetSent(){
    _responseOffsetSent = 0;
}

Client::~Client(){}

Client::Client(const Client& src) : _rawBuffer(src._rawBuffer), _requestBuffer(src._requestBuffer), _responseBuffer(src._responseBuffer),
_lastActivity(src._lastActivity), _fd(src._fd), _port(src._port), _closeAfterResponse(src._closeAfterResponse),
_responseOffsetSent(src._responseOffsetSent){
   _cgiInfo = src._cgiInfo;
}

Client& Client::operator=(const Client& rhs){
    if (this != &rhs){
        _fd = rhs._fd;
        _port = rhs._port;
        _lastActivity = rhs._lastActivity;
        _rawBuffer = rhs._rawBuffer;
        _requestBuffer = rhs._requestBuffer;
        _responseBuffer = rhs._responseBuffer;
        _closeAfterResponse = rhs._closeAfterResponse;
        _cgiInfo = rhs._cgiInfo;
        _responseOffsetSent = rhs._responseOffsetSent;
    }
    return *this;
}

id_t        Client::getPort()const{
    return _port;
}

size_t      Client::getRequestSize()const{
    size_t requestSize = _rawBuffer.find("\r\n\r\n");
    if (requestSize == std::string::npos){
        return 0;
    }
    requestSize += 4;
    if (hasContentLengthHeader()){
        ssize_t bodySize = getContentLenthSize();
        if (bodySize < 0){
            return requestSize;
        }
        requestSize += (size_t)bodySize;
    }
    return requestSize;
}


void Client::extractRequest(){
    _requestSize = getRequestSize();
    store(_rawBuffer.substr(0, _requestSize), REQUEST);
     _rawBuffer.erase(0, _requestSize); 
}
