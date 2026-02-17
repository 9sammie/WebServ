#include "Config.hpp"

Config::Config() {}

Config::Config(const Config& other)
{
	_servers = other._servers;
}

Config& Config::operator=(const Config& other)
{
	if (this != &other)
		_servers = other._servers;
}

Config::~Config() {}

int Config::parseFile(const std::string& path)
{
	(void)path;
	return 0;
}

const std::vector<ServerConfig>& Config::getServers() const
{
	return(this->_servers);
}