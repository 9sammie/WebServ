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
#include "Signal.hpp"

ServerManager::ServerManager(std::list<int> ports){
    _listeners.reserve(ports.size()); //Avoid reallocation on iteration that would destroy firsts objects and kill sockets
    _pollFds .reserve(MAX_CLIENTS + ports.size());
    for (std::list<int>::const_iterator it = ports.begin(); it != ports.end(); ++it)
    {
        _listeners.push_back(new TcpListener (*it));
        try{
            _listeners.back()->init();
        }
        catch (std::exception& e){
            std::cerr << e.what() << std::endl;
        }
        struct pollfd tempStructPollFd;
        tempStructPollFd.fd = _listeners.back()->getFd();
        tempStructPollFd.events = POLLIN;
        tempStructPollFd.revents = 0;
        _pollFds.push_back(tempStructPollFd);
    }
    _portsQuantity = _listeners.size();
    //Debug
    std::cout << GREEN << "_listeners ready with " << _portsQuantity << " different ports." << RESET << std::endl;
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
    _clients[newFd] = Client(newFd);
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
            Client& tempClient = _clients[clientFd];
            tempClient.store(std::string(tempBuff, bytesRead), Client::REQUEST);
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

void ServerManager::sendResponse(int clientFd, int idx) {
    const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";// TEMP, cooker needed

    if (send(clientFd, response, std::strlen(response), MSG_NOSIGNAL) < 0){
        closeConnection(clientFd);
    }
    // send(clientFd, response, std::strlen(response), 0); // On MacOS, and change in the main, uncomment signal()...
    _clients[clientFd].clean(Client::REQUEST);
    _pollFds[idx].events = POLLIN;
}

bool    ServerManager::isListener(int fd){
    for (std::vector<TcpListener*>::const_iterator it = _listeners.begin(); it != _listeners.end(); ++it){
        if ((*it)->getFd() == fd)
            return true;
    }
    return false;
}


/************************************************************************************************************ */
/*                                                                                                            */
/*                                                                                                            */
/*                                Refactoring functions                                                       */
/*                                                                                                            */
/*                                                                                                            */
/*                                                                                                            */
/************************************************************************************************************ */


bool    ServerManager::handleRequest(int idx){
    int fd = _pollFds[idx].fd;

    if (readClientData(fd) <= 0)
        return false;
    if (_clients[fd].isRequestComplete()){
        std::cout << BRIGHT_BLUE << "DEBUG: REQUEST complete !" << RESET << std::endl;
        // COOKER call DEBUG
        // CgiInfo
        if (_clients[fd].getCgiInfo().isCgi == true){
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
    }
    return true;
}

// -1 could be changed to 1000 to allow servermanager to clean innactive clients
void    ServerManager::run(){
    while(true){
        //Event go through _pollFds to find the revent On
        if (poll(&_pollFds[0], _pollFds.size(), -1) >= 0){
            for (size_t i = 0; i < _pollFds.size(); ++i){
                if (_pollFds[i].revents & POLLIN){ // Can read
                    if (isListener(_pollFds[i].fd)){
                        acceptNewConnection(_pollFds[i].fd);
                    }
                    // else if (_cgiClient handler, get response from cgi and store inside client)
                    else if (_cgiReadFds.count(_pollFds[i].fd)){
                        readCgiResponse();
                        //CookCgi()
                    }
                    else{
                        if (!handleRequest(i))
                            i--; //A client has been disconnected, decrement i because _pollFds has one client less
                    }
                }
                else if (_pollFds[i].revents & POLLOUT){ // Can write
                    // if () Is cgiPOLLOUT (body needs to be write into pipeWrite)
                    // write body inside pipeWrite. (client.getCgiInfo.pipeWrite)
                    // IsWritefull ? close pipe/ remove from _pollFds and _cgiWrite
                    // else{ sendResponse()}
                    if (_cgiWriteFds.count(_pollFds[i].fd)){
                        writeCgiBody();
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

void    ServerManager::removeWritePipe(int pipeWrite){
    if (close(pipeWrite) == -1)
        std::cerr << RED << "Error: close(" << pipeWrite <<") failed: " << strerror(errno) << RESET << std::endl;
    _cgiWriteFds.erase(pipeWrite);
    for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it){
        if (it->fd == pipeWrite){
            _pollFds.erase(it);
            break;
        }
    }
}

void    ServerManager::removeReadPipe(int pipeRead){
    if (close(pipeRead) == -1)
        std::cerr << RED << "Error: close(" << pipeRead <<") failed: " << strerror(errno) << RESET << std::endl;
    _cgiReadFds.erase(pipeRead);
    for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it){
        if (it->fd == pipeRead){
            _pollFds.erase(it);
            break;
        }
    }
}

// int ServerManager::findPipeReadByClient(int clientFd){
//     for (std::map<int, int>::iterator it = _cgiReadFds.begin(); it != _cgiReadFds.end(); ++it){
//         if (it->second == clientFd)
//             return it->first;
//     }
//     return -1;
// }


void    ServerManager::writeCgiBody(size_t& idx){
    /*
        idx is for i in _pollFds[i]
        pipeWrite is where to write the body of the request
        get body by using getBuffer(request), function from client.
        extract only the body and write body inside of the pipeWrite.
        Close pipeWrite
        remove from _cgiWrite IF all writen  

    */
   int pipeWrite = _pollFds[idx].fd;
   int clientFd = _cgiWriteFds[pipeWrite];
   std::string body = getBody(_clients[clientFd].getBuffer(Client::REQUEST));
    if (body.empty()){
        removeWritePipe(pipeWrite);
        --idx;
        return ;
    }
   ssize_t bytesWritten = write(pipeWrite, body.c_str(), body.size());
    if (bytesWritten > 0){
        removeWritePipe(pipeWrite);
        --idx;
        return ;
    }
    else {
        if (errno == EPIPE)
        {
            removeWritePipe(pipeWrite);
            int pipeRead = _clients[clientFd].getCgiInfo().pipeRead;
            removeReadPipe(pipeRead);
            idx -= 2;
            //IMPORTANT HERE ADD A sendResponse Send a 500 Internal Server Error to the client
            return ;
        }
        else{
            waitpid(_clients[clientFd].getCgiInfo().pid, NULL, WNOHANG);
            return ;
        }
    }
}
