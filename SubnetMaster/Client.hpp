#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <ctime>

class Client{

    public :
        Client();
        Client(int fd);   
        Client(const Client& src);
        Client& operator=(const Client& rhs);
        ~Client();

        enum BufferType {REQUEST, RESPONSE};
        bool        isRequestComplete();
        bool        hasHeadersSeparator();
        void        store(const std::string& content, BufferType type);
        void        clean(BufferType type);
        time_t      timeSinceLastActivity();
        void        updateActivity();
        int         getFd()const;
        const std::string& getBuffer(BufferType type)const;

    private :

        std::string _requestBuffer;
        std::string _responseBuffer;
        time_t      _lastActivity;
        int         _fd;
};

#endif