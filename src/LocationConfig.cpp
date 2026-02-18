#include "LocationConfig.hpp"

LocationConfig::LocationConfig()
	:	path(""),
		root(""),
		index(""),
		cgiExtension(""),
		cgiPath("")
{}

LocationConfig::LocationConfig(const LocationConfig& other)
{
	path = other.path;
	root = other.root;
	index = other.index;
	cgiExtension = other.cgiExtension;
	cgiPath = other.cgiPath;
}

LocationConfig& LocationConfig::operator=(const LocationConfig& other)
{
	if (this != &other)
	{
		path = other.path;
		root = other.root;
		index = other.index;
		cgiExtension = other.cgiExtension;
		cgiPath = other.cgiPath;
	}
	return *this;
}

LocationConfig::~LocationConfig() {}
