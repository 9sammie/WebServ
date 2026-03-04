#include "UriResolver.hpp"
#include "HttpRequest.hpp"

std::string extractPath(const std::string& uri);
std::string urlDecode(const std::string& path);
std::string normalizePath(const std::string& path);
std::string applyLocation(const std::string& path, ...);
bool isPathSecure(const std::string& fullPath, const std::string& root);
UriResolver(const ServerConfig& config);

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
		return uriResult::INVALID_PATH;
	return fullPath;
}
