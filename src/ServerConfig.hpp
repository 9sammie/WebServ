#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "LocationConfig.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>

class ServerConfig
{
	public:
		ServerConfig();
		ServerConfig(const ServerConfig& other);
		ServerConfig& operator=(const ServerConfig& other);
		~ServerConfig();
	
		int port;
		std::string host;
		std::string serverName;
		std::string root;
		std::map<int, std::string> errorPages;
		std::vector<LocationConfig> locations;
};

#endif