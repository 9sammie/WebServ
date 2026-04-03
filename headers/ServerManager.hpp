#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include "TcpListener.hpp"
#include "Client.hpp"
#include <vector>
#include <map>
#include <poll.h>
#include <list>
#include "Config.hpp"
#include <sys/types.h>

#define MAX_CLIENTS    1000
#define CLIENT_TIMEOUT 5
#define CGI_TIMEOUT    5
class ServerManager{

    public :
        ServerManager(const HttpConfig& HttpConfig/*std::list<int> ports*/);
        ~ServerManager();

        void    run();
        bool    receivedRequest(int idx);
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
        const ServerConfig& getServer(int port)const;

        //Functions to pas the good server infos to requestHandler
        std::string getServerName(const std::string& body)const;

/*******************************************************************************/
/*                            TimeOut Functions                                */
/*******************************************************************************/
        void   checkCgiTimeOuts();
        void   checkClientTimeOuts();

    private :

        const HttpConfig            _httpConfig;
        std::vector<TcpListener*>   _listeners;
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