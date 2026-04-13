#include "Config.hpp"

ListenConfig::ListenConfig()
	: host("0.0.0.0")
	, port(0)
{}

LocationConfig::LocationConfig()
	: prefix("")
	, root("")
	, alias("")
	, index("")
	, autoindex(false)
	, uploadAuthorised(false)
	, cgiAuthorised(false)
	, cgiExt("")
	, cgiPath("")
	, hasRedirection(false)
	, redirectCode(0)
	, redirectTarget("")
	, hasMaxBodySize(false)
	, maxBodySize(0)
	, hasCgiTimeout(false)
	, cgiTimeoutSec(0)
{
	methods.push_back("GET");
}

ServerConfig::ServerConfig()
	: hasKeepalive(false)
	, keepaliveTimeoutSec(0)
	, listens()
	, serverName("")
	, hasMaxBodySize(false)
	, maxBodySize(0)
	, errors()
	, root("")
	, index("")
	, locations()
{}

HttpConfig::HttpConfig()
	: hasKeepalive(true)
	, keepaliveTimeoutSec(75)
	, hasMaxBodySize(true)
	, maxBodySize(1048576)
	, errors()
	, servers()
{}
