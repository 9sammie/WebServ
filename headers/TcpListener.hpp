#ifndef TCP_LISTENER
#define TCP_LISTENER

#include <netinet/in.h> // Pour sockaddr_in
#include <string>

class TcpListener{

    public :

        TcpListener(int port);
        ~TcpListener();
    
        void    init();// Init the socket(), socketaddr();, bind() -> bind our fd to our address form, fcntl() for non blocking, listen() -> _fd becomes a listening socket
        int     getFd()const;
        int     getPort()const;

    private :

        struct sockaddr_in  _address;
        int                 _fd;
        int                 _port;

        //No use of those, why would i make a copy of a _fd ? if i close the first one, both will be closed and it
        // could lead to big issues
        TcpListener();
        TcpListener(TcpListener const & src);
        TcpListener& operator=(TcpListener const & rhs);
};

#endif