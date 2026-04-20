#include "Client.hpp"
#include "Colors.hpp"
#include <iostream>
#include <cerrno>
#include <cstdlib>

Client::Client() : _fd(-1), _serverPort(-1), _clientPort(-1), _badRequest(false), _closeStatus(false), _responseOffsetSent(0), _requestSize(0), _chunkSize(-1), _transferEncoding(false), _keepaliveTimeout(-1), _cgiTimeout(-1), _serverName("Unknown"){
    _lastActivity = time(NULL);
    _cgiInfo.isCgi = false;
    _cgiInfo.pid = -1;
    _cgiInfo.pipeRead = -1;
    _cgiInfo.pipeWrite = -1;
    _cgiInfo.start_time = 0;
    _cgiInfo.bodyWrittenBytes = 0;
}

Client::Client(int fd, int serverPort, int clientPort, std::string remoteAddr): _fd(fd),_serverPort(serverPort),
_clientPort(clientPort), _remoteAddr(remoteAddr), _badRequest(false), _closeStatus(false), _responseOffsetSent(0), _requestSize(0),
_chunkSize(-1), _transferEncoding(false), _keepaliveTimeout(-1), _cgiTimeout(-1), _serverName("Unknown"){
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


bool Client::hasTransferEncodingHeader()const{
    std::string headerRequest = _rawBuffer.substr(0, _rawBuffer.find("\r\n\r\n"));
    for (size_t i = 0; i < headerRequest.size(); ++i)
        headerRequest[i] = std::tolower((unsigned char)headerRequest[i]);
    if (headerRequest.find("transfer-encoding:") != std::string::npos)
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

bool Client::isRequestComplete(){
    if (_transferEncoding == false){
        if (!hasHeadersSeparator())
            return false;
        if (!hasContentLengthHeader() && !hasTransferEncodingHeader()){
            return true;
        }
        if (hasContentLengthHeader() && hasTransferEncodingHeader()){
            _badRequest = true;
            _closeStatus = true;
            return true;
        }
        if (hasTransferEncodingHeader()){
            _transferEncoding = true;
            size_t size = _rawBuffer.find("\r\n\r\n") + 4;
            store(_rawBuffer.substr(0, size),Client::REQUEST);
            _rawBuffer.erase(0, size);
        }
        if (hasContentLengthHeader()){
            ssize_t bodySize = getContentLenthSize();
            if (bodySize < 0){
                _badRequest = true;
                _closeStatus = true;
                return true;
            }
            return availableDataAfterHeaders() >= (size_t)bodySize;
        }
    }
    while (_transferEncoding == true) {
        if (_chunkSize == -1) {
            if (_rawBuffer.find("\r\n") == std::string::npos) {
                break;  // Waiting for the data
            }
            ssize_t ret = updateChunkSize();
            if (ret < 0) {
                _transferEncoding = false;
                _badRequest = true;
                _closeStatus = true;
                return true;
            }
        }
        else if (_chunkSize > 0) {
            ssize_t ret = getChunkData();
            if (ret == 0)
                break;
            if (ret < 0) {
                _transferEncoding = false;
                _badRequest = true;
                _closeStatus = true;
                return true;
            }
        } else {
            if (finalChunkReceived()) {
                _transferEncoding = false;
                return true;
            }
            break;
        }
    }
    return false;
}

bool Client::getRequestStatus()const{
    return _badRequest;
}

void Client::setRequestStatus(bool value){
	_badRequest = value;
}


bool Client::getCloseStatus()const{
    return _closeStatus;
}

void Client::setCloseStatus(bool value){
    _closeStatus = value;
}

int Client::getFd() const{
    return _fd;
}

const std::string& Client::getBuffer(BufferType type){
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

void				Client::setCgiInfo(CgiInfo& info){
	_cgiInfo = info;
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
_lastActivity(src._lastActivity), _fd(src._fd), _serverPort(src._serverPort), _clientPort(src._clientPort), _remoteAddr(src._remoteAddr), 
_badRequest(src._badRequest), _closeStatus(src._closeStatus), _responseOffsetSent(src._responseOffsetSent), _chunkSize(src._chunkSize),
_transferEncoding(src._transferEncoding), _keepaliveTimeout(src._keepaliveTimeout), _cgiTimeout(src._cgiTimeout), _serverName(src._serverName){
   _cgiInfo = src._cgiInfo;
}

Client& Client::operator=(const Client& rhs){
    if (this != &rhs){
        _fd = rhs._fd;
        _serverPort = rhs._serverPort;
        _clientPort = rhs._clientPort;
        _lastActivity = rhs._lastActivity;
        _rawBuffer = rhs._rawBuffer;
        _requestBuffer = rhs._requestBuffer;
        _responseBuffer = rhs._responseBuffer;
        _badRequest = rhs._badRequest;
        _closeStatus = rhs._closeStatus;
        _cgiInfo = rhs._cgiInfo;
        _responseOffsetSent = rhs._responseOffsetSent;
        _chunkSize = rhs._chunkSize;
        _transferEncoding = rhs._transferEncoding;
        _remoteAddr = rhs._remoteAddr;
        _keepaliveTimeout = rhs._keepaliveTimeout;
        _cgiTimeout = rhs._cgiTimeout;
        _serverName = rhs._serverName;

    }
    return *this;
}

int        Client::getPort(PortType type)const{
    if (type == SERVER)
        return _serverPort;
    return _clientPort;
}

std::string Client::getRemoteAddr()const{
    return _remoteAddr;
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


ssize_t     Client::updateChunkSize(){
    size_t strSize = _rawBuffer.find("\r\n");
    if (strSize == std::string::npos)
        return -1;
    std::string chunkStr = _rawBuffer.substr(0, strSize);
    char* endptr;
    long val = strtol(chunkStr.c_str(), &endptr, 16);
    while (*endptr == ' ' || *endptr == '\t')
        endptr++;
    if (endptr != chunkStr.c_str() + chunkStr.size() || val < 0)
        return -1;
    if (val != 0)
        _rawBuffer.erase(0, strSize +2);
    _chunkSize = static_cast<ssize_t>(val);
    return static_cast<ssize_t>(val);
}

ssize_t Client::getChunkData(){
    if (_rawBuffer.size() < static_cast<size_t>(_chunkSize + 2))
        return 0;  // Not enough Data
    if (_rawBuffer[_chunkSize] == '\r' && _rawBuffer[_chunkSize + 1] == '\n'){
        std::string chunkStr = _rawBuffer.substr(0, _chunkSize);
        store(chunkStr, REQUEST);
        _rawBuffer.erase(0, _chunkSize + 2);
        _chunkSize = -1;
        return 1;
    }
    return -1; 
}

bool     Client::finalChunkReceived(){
    size_t strSize = _rawBuffer.find("\r\n\r\n");
    if (strSize == std::string::npos)
        return false;
    _rawBuffer.erase(0, strSize +4);
    _chunkSize = -1;
    return true;
}

int Client::getKeepaliveTimeout()const{
    return _keepaliveTimeout;
}

void    Client::setKeepaliveTimeout(int timeout){
    _keepaliveTimeout = timeout;
}

int Client::getCgiTimeout()const{
    return _cgiTimeout;
}

void Client::setCgiTimeout(int timeout){
    _cgiTimeout = timeout;
}

void    Client::setServerName(std::string serverName){
    _serverName = serverName;
}

const std::string & Client::getServerName()const{
    return _serverName;
}
