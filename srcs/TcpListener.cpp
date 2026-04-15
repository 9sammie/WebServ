#include "TcpListener.hpp"
#include <cstring>
#include <unistd.h>
#include <stdexcept>
#include <cerrno> 
#include <fcntl.h> 
/*
Récupérer l'IP du fichier .conf (sous forme de chaîne std::string comme "127.0.0.1").
La convertir avec inet_addr() ou inet_pton() pour la passer à _address.sin_addr.s_addr à la place de htonl(INADDR_ANY).


*/
TcpListener::TcpListener(int port) : _fd(-1), _port(port){
    std::memset(&_address, 0, sizeof(_address));
    _address.sin_family = AF_INET; // IPv4
    _address.sin_addr.s_addr = htonl(INADDR_ANY); // Listen everything on the subnet, htonl for better practice
    _address.sin_port = htons(_port); //htons Host to Network short, convert in Big endian
}

TcpListener::~TcpListener(){
    if (_fd != -1)
        close(_fd);
}

// Init the socket(), socketaddr();, bind() -> bind our fd to our address form, 
// fcntl() for non blocking, listen() -> _fd becomes a listening socket
//
void TcpListener::init(){
    _fd = socket(AF_INET, SOCK_STREAM, 0);//SOCK_STREAM = TCP
    if (_fd == -1)
        throw std::runtime_error(strerror(errno));
    int opt = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        close(_fd);
        throw std::runtime_error(strerror(errno));
    }
    if (bind(_fd, (struct sockaddr*)&_address, sizeof(_address)) < 0){
        close(_fd);
        throw std::runtime_error(strerror(errno));
    }
    if (listen(_fd, 1024) < 0){
        close(_fd);
        throw std::runtime_error(strerror(errno));
    }
    if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0){
        close(_fd);
        throw std::runtime_error(strerror(errno));
    }
}

int TcpListener::getFd()const{
    return _fd;
}

int TcpListener::getPort()const{
    return _port;
}