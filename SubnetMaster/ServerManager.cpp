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
    for (std::vector<TcpListener*>::iterator it; it != _listeners.end(); ++it){
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
    close(clientFd);
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
    _pollFds[idx].events = POLLIN;
}

bool    ServerManager::isListener(int fd){
    for (std::vector<TcpListener*>::const_iterator it = _listeners.begin(); it != _listeners.end(); ++it){
        if ((*it)->getFd() == fd)
            return true;
    }
    return false;
}

bool    ServerManager::handleRequest(int idx){
    int fd = _pollFds[idx].fd;

    if (readClientData(fd) <= 0)
        return false;
    if (_clients[fd].isRequestComplete()){
        // COOCKER call
        _pollFds[idx].events = POLLOUT;
    }
    return true;
}

// -1 could be changed to 1000 to allow servermanager to clean innactive clients
void    ServerManager::run(){
    while(true){
        //Event go through _pollFds to find the revent On
        if (poll(&_pollFds[0], _pollFds.size(), -1) >= 0){
            for (size_t i = 0; i < _pollFds.size(); ++i){
                if (_pollFds[i].revents & POLLIN){
                    //Can read
                    if (isListener(_pollFds[i].fd)){// STOPPED HERE, utiliser isListenner pour ajuster
                        acceptNewConnection(_pollFds[i].fd);
                    }
                    else{
                        // if (readClientData(_pollFds[i].fd) <= 0)
                        //     i--; //A client has been disconnected, decrement i because _pollFds has one client less
                        // else // For tests ONLY, delete after simple tests
                        //     _pollFds[i].events = POLLIN | POLLOUT; // For tests ONLY, delete after simple tests
                        if (!handleRequest(i))
                            i--;
                    }
                }
                else if (_pollFds[i].revents & POLLOUT){
                    // Can write
                    sendResponse(_pollFds[i].fd, i);
                }
            }
        }
        else{
            if (errno == EINTR)
                continue;
            else
                throw std::runtime_error(strerror(errno));
        }
    }
}