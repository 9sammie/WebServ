#include "UriResolver.hpp"
#include "HttpRequest.hpp"

UriResolver::UriResolver(const ServerConfig& config): _config(config) {}

UriResolver::~UriResolver() {}


std::string extractPath(const std::string& uri);
std::string urlDecode(const std::string& path);
std::string normalizePath(const std::string& path);
std::string applyLocation(const std::string& path, ...);
bool isPathSecure(const std::string& fullPath, const std::string& root);


std::string UriResolver::resolve(const HttpRequest& request)
{
	std::string path;
	std::string fullPath;
	const LocationConfig* loc;

	path = extractPath(request.getUri());
	path = urlDecode(path);
	path = normalize(path);
	loc = findMatchingLocation(path);
	fullPath = applyRootOrAlias(path, loc);
	if (!isSecure(fullPath))
		throw HttpException(203, "invalid path");
	return fullPath;
}
