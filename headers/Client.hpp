#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <ctime>
#include <sys/types.h>

class Client{

    public :
        Client();
        Client(int fd, int serverPort, int clientPort, std::string remoteAddr);   
        Client(const Client& src);
        Client& operator=(const Client& rhs);
        ~Client();

        enum        BufferType {REQUEST, RESPONSE, RAW};
        enum        PortType {SERVER, CLIENT};
        typedef struct CgiInfo{
            bool    isCgi;
            int     pipeRead;
            int     pipeWrite;
            size_t  bodyWrittenBytes;
            pid_t   pid;
            time_t  start_time;
        }CgiInfo;
        bool        isRequestComplete();
        bool        hasHeadersSeparator();
        bool        hasBody();
        bool        hasContentLengthHeader()const;
        bool        hasTransferEncodingHeader()const;
        bool        getRequestStatus()const;
        void        setRequestStatus(bool value);
        bool        getCloseStatus()const;
        void        setCloseStatus(bool value);
        ssize_t     getContentLenthSize()const;
        size_t      availableDataAfterHeaders();
        void        store(const std::string& content, BufferType type);
        void        clean(BufferType type);
        time_t      timeSinceLastActivity();
        void        updateActivity();

        int         getFd()const;
        CgiInfo&    getCgiInfo();
		void		setCgiInfo(CgiInfo& info);
        int         getPort(PortType type)const;
        std::string getRemoteAddr()const;
        const std::string& getBuffer(BufferType type);
        int         getKeepaliveTimeout()const;
        void        setKeepaliveTimeout(int timeout);
        int         getCgiTimeout()const;
        void        setCgiTimeout(int timeout);

        void        resetCgiInfos();
        size_t&     getResponseOffsetSent();
        void        resetResponseOffsetSent();
        void        extractRequest();
        size_t      getRequestSize()const;
        ssize_t     updateChunkSize();
        ssize_t     getChunkData();
        bool        finalChunkReceived();


    private :

        std::string _rawBuffer;
        std::string _requestBuffer;
        std::string _responseBuffer;
        time_t      _lastActivity;
        int         _fd;
        CgiInfo     _cgiInfo;
        int         _serverPort;
        int         _clientPort;
        std::string _remoteAddr;
        bool        _badRequest;
        bool        _closeStatus;
        size_t      _responseOffsetSent;
        size_t      _requestSize;
        ssize_t     _chunkSize;
        bool        _transferEncoding;
        int         _keepaliveTimeout;
        int         _cgiTimeout;
};

#endif