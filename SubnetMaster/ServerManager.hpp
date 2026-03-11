#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include "TcpListener.hpp"
#include "Client.hpp"
#include <vector>
#include <map>
#include <poll.h>
#include <list>

#define MAX_CLIENTS    1000
#define CLIENT_TIMEOUT 60
#define CGI_TIMEOUT    60
class ServerManager{

    public :
        ServerManager(std::list<int> ports);
        ~ServerManager();

        void    run();
        bool    handleRequest(int idx);
        void    acceptNewConnection(int serverFd);
        void    closeConnection(int clientFd);
        int     readClientData(int clientFd);
        void    sendResponse(int clientFd, int idx);
        void    readCgiResponse(size_t& idx);
        void    writeCgiBody(size_t& idx);
        size_t  removeWritePipe(int pipeWrite);
        size_t  removeReadPipe(int pipeRead);
        void    setPollout(int clientFd);
        // int     findPipeReadByClient(int clientFd);
        bool    isListener(int fd);
        int     getListenerPort(int fd);

/*******************************************************************************/
/*                            TimeOut Functions                                */
/*******************************************************************************/
        void   checkCgiTimeOuts();
        void   checkClientTimeOuts();

    private :

        std::vector<TcpListener*>    _listeners;
        std::map<int, Client>       _clients;
        //       fd, clientFd
        std::map<int, int>          _cgiReadFds;
        std::map<int, int>          _cgiWriteFds;
        std::vector<struct pollfd>  _pollFds;
        int                         _portsQuantity;

        // UNUSED Canonical form
        ServerManager();
        ServerManager(const ServerManager& src);
        ServerManager& operator=(const ServerManager& rhs);

};

#endif