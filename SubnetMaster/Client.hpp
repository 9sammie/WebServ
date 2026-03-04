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

        enum        BufferType {REQUEST, RESPONSE};
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
        bool        hasContentLengthHeader();
        ssize_t     getContentLenthSize();
        size_t      actualBodySize();
        void        store(const std::string& content, BufferType type);
        void        clean(BufferType type);
        time_t      timeSinceLastActivity();
        void        updateActivity();
        int         getFd()const;
        const std::string& getBuffer(BufferType type);
        CgiInfo&    getCgiInfo();

    private :

        std::string _requestBuffer;
        std::string _responseBuffer;
        time_t      _lastActivity;
        int         _fd;
        bool        _closeAfterResponse;
        CgiInfo     _cgiInfo;
        id_t        _port;
};

#endif