#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <string>
#include <vector>
#include <map>
#include <cstddef>

struct ListenConfig
{
	ListenConfig();
	std::string host;
	int port;
};

struct LocationConfig
{
	LocationConfig();
	std::string prefix;
	std::string root;
	std::string alias; 
	std::string index;
	bool autoindex;
	std::vector<std::string> methods;
	bool uploadAuthorised;
	bool cgiAuthorised;
	std::string cgiExt;
	std::string cgiPath;
	bool hasRedirection;
	int redirectCode;
	std::string redirectTarget;
	bool hasMaxBodySize;
	std::size_t maxBodySize;
	bool hasCgiTimeout;
	int cgiTimeoutSec;
};

struct ServerConfig
{
	ServerConfig();
	bool hasKeepalive;
	int keepaliveTimeoutSec;
	std::vector<ListenConfig> listens;
	std::string serverName;
	bool hasMaxBodySize;
	std::size_t maxBodySize;
	std::map<int, std::string> errors; 
	std::string root;
	std::string index;
	std::vector<LocationConfig> locations;
};

struct HttpConfig
{
	HttpConfig();
	bool hasKeepalive;
	int keepaliveTimeoutSec;
	bool hasMaxBodySize;
	std::size_t maxBodySize;
	std::map<int, std::string> errors;
	std::vector<ServerConfig> servers;
};

#endif
