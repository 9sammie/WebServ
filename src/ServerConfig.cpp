#include "ServerConfig.hpp"

ServerConfig::ServerConfig() 
	:	port(80),
		host(""),
		serverName(""),
		root("")
{}

ServerConfig::ServerConfig(const ServerConfig& other)
{
	port = other.port;
	host = other.host;
	serverName = other.serverName;
	root = other.root;
	errorPages = other.errorPages;
	locations = other.locations;
}

ServerConfig& ServerConfig::operator=(const ServerConfig& other)
{
	if (this != &other)
	{
		port = other.port;
		host = other.host;
		serverName = other.serverName;
		root = other.root;
		errorPages = other.errorPages;
		locations = other.locations;
	}
}

ServerConfig::~ServerConfig() {}