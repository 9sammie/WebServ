#ifndef URIRESOLVER_HPP
#define URIRESOLVER_HPP

#include "HttpRequest.hpp"
#include "Config.hpp"

class UriResolver
{
private:
	const ServerConfig&		_config;

	std::string				extractPath(const std::string& uri);
	std::string				urlDecode(const std::string& path);
	std::string				normalize(const std::string& path);
	const LocationConfig*	UriResolver::findMatchingLocation(const std::string& path);
	std::string				applyRootOrAlias(const std::string& path, const LocationConfig*);
	bool					isPathSecure(const std::string& fullPath);

	public:
	UriResolver(const ServerConfig& config);
	~UriResolver();

	std::string				resolve(const HttpRequest& request);
};

#endif
