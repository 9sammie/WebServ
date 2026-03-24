#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <ctime>
#include <sys/types.h>

class Client{

    public :
        Client();
        Client(int fd, int port);   
        Client(const Client& src);
        Client& operator=(const Client& rhs);
        ~Client();

        enum        BufferType {REQUEST, RESPONSE, RAW};
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
        ssize_t     getContentLenthSize()const;
        size_t      availableDataAfterHeaders();
        void        store(const std::string& content, BufferType type);
        void        clean(BufferType type);
        time_t      timeSinceLastActivity();
        void        updateActivity();
        int         getFd()const;
        const std::string& getBuffer(BufferType type);
        CgiInfo&    getCgiInfo();
        id_t        getPort()const;
        void        resetCgiInfos();
        size_t&     getResponseOffsetSent();
        void        resetResponseOffsetSent();
        void        extractRequest();
        size_t      getRequestSize()const;


    private :

        std::string _rawBuffer;
        std::string _requestBuffer;
        std::string _responseBuffer;
        time_t      _lastActivity;
        int         _fd;
        CgiInfo     _cgiInfo;
        id_t        _port;
        bool        _closeAfterResponse;
        size_t      _responseOffsetSent;
        size_t      _requestSize;
};

#endif