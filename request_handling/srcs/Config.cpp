#include "Config.hpp"

ListenConfig::ListenConfig()
	: host("0.0.0.0")
	, port(0)
{}

LocationConfig::LocationConfig()
	: autoindex(false)
	, uploadAuthorised(false)
	, cgiAuthorised(false)
	, hasRedirection(false)
	, redirectCode(0)
	, hasMaxBodySize(false)
	, maxBodySize(0)
{
	methods.push_back("GET"); //GET si rien n est precise, a reflechir si pertinent, gestion doublon...
}

ServerConfig::ServerConfig()
	: keepaliveTimeoutSec(15)
	, hasMaxBodySize(false)
	, maxBodySize(0)
{}

HttpConfig::HttpConfig() 
	: keepaliveTimeoutSec(15)
	, maxBodySize(1048576)
{}