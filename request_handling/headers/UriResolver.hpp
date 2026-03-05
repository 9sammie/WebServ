#ifndef URIRESOLVER_HPP
#define URIRESOLVER_HPP

#include "HttpRequest.hpp"
#include "Config.hpp"

class UriResolver
{
private:
	const ServerConfig& _config;

	std::string extractPath(const std::string& uri);
	std::string urlDecode(const std::string& path);
	std::string normalize(const std::string& path);
	const LocationConfig* findMatchingLocation(const std::string& path);
	std::string applyRootOrAlias(const std::string& path, const LocationConfig*);
	bool isSecure(const std::string& fullPath);

	public:
	void UriResolver(const ServerConfig& config);

	std::string resolve(const HttpRequest& request);
};

#endif
