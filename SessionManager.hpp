#ifndef SESSIONMANAGER_HPP
#define SESSIONMANAGER_HPP
#include <map>
#include <ctime>
#include <string>
# define COOKIE_MAX_AGE 1800 // 30 mins in seconds
typedef struct cookie_s
{
    std::string id;
    std::time_t expDate;
    std::time_t lastAccess;
    std::time_t createdAt;
} cookie_t;

class SessionManager
{
    public:
        SessionManager();
        SessionManager(const SessionManager& src);
        SessionManager& operator=(const SessionManager& src);
        ~SessionManager();
        //fonctions :
    private:
	    std::map<std::string, cookie_t>	_sessions;
	    std::string	generateSessionId() const;
	    bool    isExpired(const cookie_t& cookie) const;
};
#endif