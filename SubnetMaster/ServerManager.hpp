#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include "TcpListener.hpp"
#include "Client.hpp"
#include <vector>
#include <map>
#include <poll.h>
#include <list>

#define MAX_CLIENTS 1000
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

        bool    isListener(int fd);

    private :

        std::vector<TcpListener*>    _listeners;
        std::map<int, Client>       _clients;
        std::vector<struct pollfd>  _pollFds;
        int                         _portsQuantity;

        // UNUSED Canonical form
        ServerManager();
        ServerManager(const ServerManager& src);
        ServerManager& operator=(const ServerManager& rhs);

};

#endif