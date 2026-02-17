#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "ServerConfig.hpp"

class LocationConfig
{
	public:
	LocationConfig();
	LocationConfig(const LocationConfig& other);
	LocationConfig& operator=(const LocationConfig& other);
	~LocationConfig();

	std::string path;
    std::string root;
    std::string index;
    std::string cgiExtension;
    std::string cgiPath;
    std::vector<std::string> allowedMethods;
};

#endif