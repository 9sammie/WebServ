#include "ServerManager.hpp"
#include "Client.hpp"
#include "TcpListener.hpp"
#include <exception>
#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include "Colors.hpp"
#include <sys/wait.h>
#include "Signal.hpp"
#include <set>
#include "RequestHandler.hpp"
#include <string>
#include <sstream>
#include "RequestHandler.hpp"
#include <arpa/inet.h>
#include "CgiResponseProcessor.hpp"

ServerManager::ServerManager(const HttpConfig& httpConfig) : _httpConfig(httpConfig){
    std::set<int> ports;
    for (std::vector<ServerConfig>::const_iterator it = _httpConfig.servers.begin(); it != _httpConfig.servers.end(); ++it){
        for (size_t i = 0; i < it->listens.size(); ++i){
            ports.insert(it->listens[i].port);
        }
    }
    _listeners.reserve(ports.size()); //Avoid reallocation on iteration that would destroy firsts objects and kill sockets
    _pollFds .reserve(MAX_CLIENTS + ports.size());
    for (std::set<int>::const_iterator it = ports.begin(); it != ports.end(); ++it)
    {
        TcpListener* tmp = new TcpListener(*it);
        try{
            tmp->init();
            _listeners.push_back(tmp);
            struct pollfd tempStructPollFd;
            tempStructPollFd.fd = tmp->getFd();
            tempStructPollFd.events = POLLIN;
            tempStructPollFd.revents = 0;
            _pollFds.push_back(tempStructPollFd);
        }
        catch (std::exception& e){
            delete tmp;
            std::cerr << e.what() << std::endl;
        }
    }
    _portsQuantity = _listeners.size();
    if (_pollFds.empty()) {
        throw std::runtime_error("No listeners created from HttpConfig: _pollFds is empty. Check your listen directives and permissions.");
    }
    std::cout << GREEN << "_listeners ready with " << _portsQuantity << " different ports." << RESET << std::endl;//Debug
}

ServerManager::~ServerManager(){
    for (std::vector<TcpListener*>::iterator it = _listeners.begin(); it != _listeners.end(); ++it){
        delete *it;
    }
}

void ServerManager::acceptNewConnection(int serverFd){
    struct sockaddr_in address;
    socklen_t address_size = sizeof(address);

    int newFd = accept(serverFd, (struct sockaddr *)&address, &address_size);
    if (newFd == -1){
        std::cerr << RED << "Error: accept() on new client failed." << RESET << std::endl;
        return ;
    }
    if (fcntl(newFd, F_SETFL, O_NONBLOCK) < 0){
        close(newFd);
        std::cerr << RED << "Error: fcntl() for O_NONBLOCK on new client failed." << RESET << std::endl;
        return ;
    }

    struct pollfd newSPollFd;

    newSPollFd.fd = newFd;
    newSPollFd.events = POLLIN;
    newSPollFd.revents = 0;
    _pollFds.push_back(newSPollFd);
    std::string remoteAddr = inet_ntoa(address.sin_addr);
    _clients[newFd] = Client(newFd, getListenerPort(serverFd), address.sin_port, remoteAddr);
    _clients[newFd].setKeepaliveTimeout(getTimeout(getListenerPort(serverFd)));
    _clients[newFd].setCgiTimeout(getCgiTimeout(getListenerPort(serverFd)));
    //Debug
    std::cout << BRIGHT_GREEN << "New client connected on: " << serverFd << "." << RESET << std::endl;
}

void    ServerManager::closeConnection(int clientFd){
    if (close(clientFd) == -1)
        std::cerr << RED << "Error: close(" << clientFd <<") failed: " << strerror(errno) << RESET << std::endl;
    _clients.erase(clientFd);
    for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it){
        if (it->fd == clientFd){
            _pollFds.erase(it);
            break;
        }
    }
}

int ServerManager::readClientData(int clientFd){
    char tempBuff[4096];

    ssize_t bytesRead = recv(clientFd, tempBuff, sizeof(tempBuff), 0);
        if (bytesRead > 0){
            _clients[clientFd].store(std::string(tempBuff, bytesRead), Client::RAW);
            _clients[clientFd].updateActivity();
            return bytesRead;
        }
        else if (bytesRead == 0){
            //Client disconnected properly close fd, remove from _clients and _pollFds
            this->closeConnection(clientFd);
            std::cout << YELLOW << "Client: [" << clientFd << "] disconnected." << RESET << std::endl;
            return 0;
        }
        else{
            //Error -1, something crashed (ex: wifi stopped, a cable was unplug...)
            this->closeConnection(clientFd);
            std::cout << RED << "Error: recv() failed on client: [" << clientFd << "]." << RESET << std::endl;
            return -1;
        }
}

void ServerManager::sendResponse(int clientFd, int idx) {// Final VERSION
    
    const std::string& response = _clients[clientFd].getBuffer(Client::RESPONSE);
    // std::cout << MAGENTA << "DEBUG: Trying to send " << response.size() << " bytes as response." << RESET << std::endl;
    // std::cout << "DEBUG : Request: [" << BLUE << _clients[clientFd].getBuffer(Client::REQUEST) << "]" << RESET << std::endl;
    // std::cout << "DEBUG : Response: [" << BROWN << _clients[clientFd].getBuffer(Client::RESPONSE) << "]" << RESET << std::endl;

    size_t& offset = _clients[clientFd].getResponseOffsetSent();
    const void* dataToSend = response.c_str() + offset;
    size_t sizeToSend = response.size() - offset;
    ssize_t sent = send(clientFd, dataToSend, sizeToSend, MSG_NOSIGNAL);

    if (sent >= 0){
        offset += sent;
        if (offset >= response.size()){
            _clients[clientFd].clean(Client::REQUEST);
            _clients[clientFd].clean(Client::RESPONSE);
            _clients[clientFd].resetResponseOffsetSent();
            _pollFds[idx].events = POLLIN;
        }
    }
    else if (sent == -1){
        if (errno == EINTR)
            return ;
        else
            closeConnection(clientFd);
    }
}

bool    ServerManager::isListener(int fd){
    for (std::vector<TcpListener*>::const_iterator it = _listeners.begin(); it != _listeners.end(); ++it){
        if ((*it)->getFd() == fd)
            return true;
    }
    return false;
}

int    ServerManager::getListenerPort(int fd){
    for (std::vector<TcpListener*>::const_iterator it = _listeners.begin(); it != _listeners.end(); ++it){
        if ((*it)->getFd() == fd)
            return (*it)->getPort();
    }
    return -1;
}

int ServerManager::getTimeout(int port)const{
    for (std::vector<ServerConfig>::const_iterator it = _httpConfig.servers.begin(); it != _httpConfig.servers.end(); ++it) {
        for (size_t i = 0; i < it->listens.size(); ++i) {
            if (it->listens[i].port == port) {
                return it->keepaliveTimeoutSec;
            }
		}
    }
    return 1;
}

int ServerManager::getCgiTimeout(int port)const{
    for (std::vector<ServerConfig>::const_iterator it = _httpConfig.servers.begin(); it != _httpConfig.servers.end(); ++it) {
        for (size_t i = 0; i < it->listens.size(); ++i) {
            if (it->listens[i].port == port) {
                for (std::vector<LocationConfig>::const_iterator itL = it->locations.begin(); itL != it->locations.end(); ++itL){
                    if (itL->prefix.find("/cgi") != std::string::npos)
                    return itL->cgiTimeoutSec;
                }
            }
		}
    }
    return 1;
}

/************************************************************************************************************ */
/*                                                                                                            */
/*                                                                                                            */
/*                                Refactoring functions                                                       */
/*                                                                                                            */
/*                                                                                                            */
/*                                                                                                            */
/************************************************************************************************************ */


void   ServerManager::checkCgiTimeOuts(){
   for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it){
        if (it->second.getCgiInfo().isCgi == true){
            int timeoutCgiVal = 5;//it->second.getCgiTimeout();
            if (timeoutCgiVal < 0)
            if (time(NULL) - it->second.getCgiInfo().start_time > timeoutCgiVal){
                kill(it->second.getCgiInfo().pid, SIGKILL);
                waitpid(it->second.getCgiInfo().pid, NULL, WNOHANG);
                removeReadPipe(it->second.getCgiInfo().pipeRead);
                if (it->second.getCgiInfo().pipeWrite != -1)
                    removeWritePipe(it->second.getCgiInfo().pipeWrite);
                it->second.resetCgiInfos();
                //Hardcoded REPSONSE for NOW
                const std::string response = RequestHandler::buildHttpResponse(504, "Gateway Timeout",
                "<html><body><h1>504 Gateway Timeout</h1></body></html>", true);
                it->second.store(response, Client::RESPONSE);
                setPollout(it->second.getFd());
            }
        }
   }
}

void   ServerManager::checkClientTimeOuts(){
   for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ){
        int timeoutVal = it->second.getKeepaliveTimeout();
        if (timeoutVal > 0 &&  it->second.timeSinceLastActivity() > timeoutVal){
            int fd = it->second.getFd();
            std::cerr << "Client: [" << fd << "] disconnected: TimeOut." << std::endl;
            ++it;
            closeConnection(fd);
        }
        else if (timeoutVal == 0 && it->second.timeSinceLastActivity() > 60){
            int fd = it->second.getFd();
            std::cerr << "Client: [" << fd << "] disconnected: TimeOut." << std::endl;
            ++it;
            closeConnection(fd);
        }
        else
            ++it;
   }
}


bool    ServerManager::receivedRequest(int idx){
    int fd = _pollFds[idx].fd;

    if (readClientData(fd) <= 0)
        return false;
    if (_clients[fd].isRequestComplete()){
        std::cout << BRIGHT_BLUE << "DEBUG: REQUEST complete !" << RESET << std::endl;
        std::cout  << "REQUEST: [" << _clients[fd].getBuffer(Client::REQUEST) << "]" << std::endl;

        _clients[fd].extractRequest();
        // COOKER call will call CgiHandler() if it's a CGI
        const ServerConfig& serverToSend = getServer(_clients[fd].getPort(Client::SERVER));
		RequestHandler RH(serverToSend);

		_clients[fd].store(RH.handleRequest(_clients[fd]), Client::RESPONSE);
        // CgiInfo
        if (_clients[fd].getCgiInfo().isCgi == true){
            // std::cout << "New cgi launched" << std::endl;
            // std::cout << "pid: " << _clients[fd].getCgiInfo().pid << std::endl;
            // std::cout << "pipeRead: " << _clients[fd].getCgiInfo().pipeRead << std::endl;
            // std::cout << "pipeWrite: " << _clients[fd].getCgiInfo().pipeWrite << std::endl;
            // std::cout << "bodyWrittenBytes: " << _clients[fd].getCgiInfo().bodyWrittenBytes << std::endl;


            int pipeRead = _clients[fd].getCgiInfo().pipeRead;
            struct pollfd cgiReadFd;
            cgiReadFd.fd = pipeRead;
            cgiReadFd.events = POLLIN;
            cgiReadFd.revents = 0;
            _pollFds.push_back(cgiReadFd);
            _cgiReadFds[pipeRead] = fd;
           if (_clients[fd].getCgiInfo().pipeWrite != -1){
                int pipeWrite = _clients[fd].getCgiInfo().pipeWrite;
                struct pollfd cgiWriteFd;
                cgiWriteFd.fd = pipeWrite;
                cgiWriteFd.events = POLLOUT;
                cgiWriteFd.revents = 0;
                _pollFds.push_back(cgiWriteFd);
                _cgiWriteFds[pipeWrite] = fd;
           }
        }
        else
            _pollFds[idx].events = POLLOUT;
    }
    else{
        std::cout << BRIGHT_RED << "DEBUG: REQUEST incomplete !" << RESET << std::endl;
        std::cout << BROWN << "Request: " << _clients[fd].getBuffer(Client::REQUEST) << RESET << std::endl;
    }
    return true;
}

// In poll() -1 has been changed to 1000 to allow servermanager to clean innactive clients
void    ServerManager::run(){
    while(true){
        checkCgiTimeOuts();
        checkClientTimeOuts();
        //Event go through _pollFds to find the revent On
        if (poll(&_pollFds[0], _pollFds.size(), 1000) >= 0){
            for (size_t i = 0; i < _pollFds.size(); ++i){
                if (_pollFds[i].revents & POLLIN){ // Can read
                    if (isListener(_pollFds[i].fd)){
                        acceptNewConnection(_pollFds[i].fd);
                    }
                    // else if (_cgiClient handler, get response from cgi and store inside client)
                    else if (_cgiReadFds.count(_pollFds[i].fd))
                        readCgiResponse(i);//CookCgi() inside readCgiResponse()
                    else{
                        if (!receivedRequest(i))
                            i--; //A client has been disconnected, decrement i because _pollFds has one client less
                    }
                }
                else if (_pollFds[i].revents & POLLOUT){ // Can write
                    if (_cgiWriteFds.count(_pollFds[i].fd)){
                        writeCgiBody(i);
                    }
                    else
                        sendResponse(_pollFds[i].fd, i);
                }
            }
        }
        else{
            if (errno == EINTR){
                if (stop_sig == 1)
                    break;
                else
                    continue;
            }
            else
                throw std::runtime_error(strerror(errno));
        }
    }
}

std::string getBody(const std::string& requestBuffer){
    size_t headerEnd = requestBuffer.find("\r\n\r\n");
    return requestBuffer.substr(headerEnd + 4);
}

size_t    ServerManager::removeWritePipe(int pipeWrite){
    if (close(pipeWrite) == -1)
        std::cerr << RED << "Error: close(" << pipeWrite <<") failed: " << strerror(errno) << RESET << std::endl;
    _cgiWriteFds.erase(pipeWrite);
    for (size_t i = 0; i < _pollFds.size(); ++i){
        if (_pollFds[i].fd == pipeWrite){
            _pollFds.erase(_pollFds.begin() + i);
            return i;
        }
    }
    return _pollFds.size();
}

size_t    ServerManager::removeReadPipe(int pipeRead){
    if (close(pipeRead) == -1)
        std::cerr << RED << "Error: close(" << pipeRead <<") failed: " << strerror(errno) << RESET << std::endl;
    _cgiReadFds.erase(pipeRead);
    for (size_t i = 0; i < _pollFds.size(); ++i){
        if (_pollFds[i].fd == pipeRead){
            _pollFds.erase(_pollFds.begin() + i);
            return i;
        }
    }
    return _pollFds.size();
}

void    ServerManager::writeCgiBody(size_t& idx){
   int pipeWrite = _pollFds[idx].fd;
   int clientFd = _cgiWriteFds[pipeWrite];
   std::string body = getBody(_clients[clientFd].getBuffer(Client::REQUEST));
    if (body.empty()){
        if (removeWritePipe(pipeWrite) <= idx)
            --idx;
        return ;
    }
    size_t offset = _clients[clientFd].getCgiInfo().bodyWrittenBytes;
    ssize_t bytesWritten = write(pipeWrite, body.c_str() + offset, body.size() - offset);
    if (bytesWritten > 0){
        _clients[clientFd].getCgiInfo().bodyWrittenBytes += bytesWritten;
        if (_clients[clientFd].getCgiInfo().bodyWrittenBytes == body.size()){
            if (removeWritePipe(pipeWrite) <= idx)
                --idx;
            return ;
        }
    }
    else if (bytesWritten == -1 && errno == EPIPE){//CGI died clean and send reponse 500
            if (removeWritePipe(pipeWrite) <= idx)
                --idx;
            int pipeRead = _clients[clientFd].getCgiInfo().pipeRead;
            if (removeReadPipe(pipeRead) <= idx)
                --idx;
            waitpid(_clients[clientFd].getCgiInfo().pid, NULL, WNOHANG);
            _clients[clientFd].store(RequestHandler::buildHttpResponse(500, "Internal Server Error", 
                "<html><body><h1>500 Internal Server Error</h1></body></html>", true), Client::RESPONSE);
            setPollout(clientFd);
            return ;
        }
    else{
        return ;
    }
}

void    ServerManager::setPollout(int clientFd){
    for (size_t i = 0; i < _pollFds.size(); ++i){
            if (_pollFds[i].fd == clientFd){
                _pollFds[i].events = POLLOUT;
                break;
            }
    }
}

const ServerConfig& ServerManager::getServer(int port) const {
    for (std::vector<ServerConfig>::const_iterator it = _httpConfig.servers.begin(); it != _httpConfig.servers.end(); ++it) {
        for (size_t i = 0; i < it->listens.size(); ++i) {
            if (it->listens[i].port == port) {
                return *it;
            }
		}
    }
	std::stringstream ss;
	ss << port;
    throw std::runtime_error("No ServerConfig found for port: " + ss.str());
}


void    ServerManager::readCgiResponse(size_t& idx){
    int pipeRead = _pollFds[idx].fd;
    int clientFd = _cgiReadFds[pipeRead];
    char buffer[4096];
    ssize_t bytesRead = read(pipeRead, buffer, 4096);
    if (bytesRead > 0)//CGI still working
    {
        // std::cout << "Buffer response: [" << _clients[clientFd].getBuffer(Client::RESPONSE) << "]" << std::endl;
        _clients[clientFd].store(std::string(buffer, bytesRead),Client::RESPONSE);
        // Try to read again to detect EOF immediately if script finished
        ssize_t bytesReadAgain = read(pipeRead, buffer, 4096);
        if (bytesReadAgain == 0) {//CGI complete
            // Script finished, process response now
            std::string result = _clients[clientFd].getBuffer(Client::RESPONSE);
            _clients[clientFd].clean(Client::RESPONSE);
            waitpid(_clients[clientFd].getCgiInfo().pid, NULL, WNOHANG);
            _clients[clientFd].store(cgiResponseProcessor(result, getServer(_clients[clientFd].getPort(Client::SERVER))), Client::RESPONSE);
            if (removeReadPipe(pipeRead) <= idx){
                std::cout << "Inside readCgiResponse call removeReadPipe on pipe: " << pipeRead << std::endl;
                --idx;
            }
            _clients[clientFd].resetCgiInfos();
            setPollout(clientFd);
        } else if (bytesReadAgain > 0) {
            _clients[clientFd].store(std::string(buffer, bytesReadAgain), Client::RESPONSE);
        }
    }
    else if (bytesRead == 0){
        std::string result = _clients[clientFd].getBuffer(Client::RESPONSE);
        _clients[clientFd].clean(Client::RESPONSE);
        waitpid(_clients[clientFd].getCgiInfo().pid, NULL, WNOHANG);
        _clients[clientFd].store(cgiResponseProcessor(result, getServer(_clients[clientFd].getPort(Client::SERVER))), Client::RESPONSE);
        if (removeReadPipe(pipeRead) <= idx)
                --idx;
        _clients[clientFd].resetCgiInfos();
        setPollout(clientFd);// Set to POLLOUT to then send response
    }
    else if (bytesRead == -1 && errno != EINTR){//Error: clean and send 500
        waitpid(_clients[clientFd].getCgiInfo().pid, NULL, WNOHANG);
        if (removeReadPipe(pipeRead) <= idx)
                --idx;
            _clients[clientFd].store(RequestHandler::buildHttpResponse(500, "Internal Server Error", 
                "<html><body><h1>500 Internal Server Error</h1></body></html>", true), Client::RESPONSE);
            setPollout(clientFd);
    }
    return ;
}