#include "UriResolver.hpp"
#include "HttpRequest.hpp"
#include <cctype>
#include <sstream>
#include <climits>
#include <cstdlib>

// Uniform Ressource Identifier
UriResolver::UriResolver(const ServerConfig& config): _config(config) {}

UriResolver::~UriResolver() {}

bool UriResolver::isPathSecure(const std::string& fullPath, const LocationConfig* loc)
{
	char actualPath[PATH_MAX];
	char rootAbs[PATH_MAX];
	std::string realFullPath;
	std::string realRoot;
	std::string dirPath;
	std::string realDir;
	size_t lastSlash;

	if (realpath(loc->root.c_str(), rootAbs) == NULL)
		return false;
	realRoot = rootAbs;

	if (realpath(fullPath.c_str(), actualPath) != NULL)
	{
		realFullPath = actualPath;
		if (realFullPath.compare(0, realRoot.size(), realRoot) == 0)
		{
			if (realFullPath.size() == realRoot.size() || realFullPath[realRoot.size()] == '/')
				return true;
		}

		return false;
	}
	else
	{
		dirPath = fullPath;
		lastSlash = dirPath.find_last_of('/');

		if (lastSlash != std::string::npos)
			dirPath = dirPath.substr(0, lastSlash);

		if (dirPath.empty())
			dirPath = ".";
		if (realpath(dirPath.c_str(), actualPath) != NULL)
		{
			realDir = actualPath;
			if (realDir.compare(0, realRoot.size(), realRoot) == 0)
			{
				if (realDir.size() == realRoot.size() || realDir[realRoot.size()] == '/')
					return true;
			}
		}

		return false;
	}
}

std::string UriResolver::applyRootOrAlias(const std::string& path, const LocationConfig* loc)
{
	std::string fullPath;
	std::string base;

	if (loc && !loc->alias.empty())
	{
		std::string prefix = loc->prefix;
		std::string alias = loc->alias;

		std::string remaining = path.substr(prefix.size());
		base = alias;
		fullPath = base + remaining;
	}
	else
	{
		base = loc? loc->root: _config.root;
		if (loc && loc->prefix != "/")
		{
			std::string remaining = path.substr(loc->prefix.size());
            fullPath = base + remaining;
		}
		else
		{
			fullPath = base + path;
		}
	}

	std::string cleanPath;
	for (size_t i = 0; i < fullPath.size(); ++i) {
		if (fullPath[i] == '/' && i + 1 < fullPath.size() && fullPath[i+1] == '/')
			continue;
		cleanPath += fullPath[i];
	}

	return cleanPath;
}

const LocationConfig* UriResolver::findMatchingLocation(const std::string& path)
{
    const LocationConfig* bestMatch = NULL;
    size_t longestLength = 0;
    const std::vector<LocationConfig>& locations = _config.locations;

    for (size_t i = 0; i < locations.size(); ++i)
    {
        const std::string& prefix = locations[i].prefix;

        if (path.compare(0, prefix.size(), prefix) == 0)
        {
            if (path.size() == prefix.size() || prefix[prefix.size() - 1] == '/' || path[prefix.size()] == '/')
            {
                if (prefix.size() >= longestLength)
                {
                    longestLength = prefix.size();
                    bestMatch = &locations[i];
                }
            }
        }
    }
    return bestMatch;
}

std::string UriResolver::normalize(const std::string& path)
{
	if (path.empty()) return "/";
    
	std::vector<std::string> segments;
	std::string segment;
	std::istringstream iss(path);

	while (std::getline(iss, segment, '/'))
	{
		if (segment == "" || segment == ".")
			continue;
		if (segment == "..")
		{
			if (!segments.empty())
				segments.pop_back();
		}
		else
			segments.push_back(segment);
	}

	std::string res = "";
	for (size_t i = 0; i < segments.size(); ++i)
		res += "/" + segments[i];

	if (res.empty())
		return "/";

    if (path[path.size() - 1] == '/' && res[res.size() - 1] != '/')
		res += "/";

    return res;
}

// Check s'il y a des caracteres encodé et on les remplacent par leur valeur en ascii
std::string UriResolver::urlDecode(const std::string& path)
{
	std::string result;
    result.reserve(path.size());

    for (size_t i = 0; i < path.size(); ++i)
    {
        if (path[i] == '%')
        {
            if (i + 2 < path.size() && isxdigit(path[i + 1]) && isxdigit(path[i + 2]))
            {
                std::string hexStr = path.substr(i + 1, 2);
                std::istringstream iss(hexStr);
                int value;
                
                if (iss >> std::hex >> value)
                {
                    result += static_cast<char>(value);
                    i += 2;
                }
				else
					throw HttpException(400, "Bad Request: Invalid hex sequence");
            }
            else
				throw HttpException (400, "Bad request: Malformed Url encoding");
        }
        else
        {
            result += path[i];
        }
    }
    return result;
}

std::string UriResolver::extractPath(const std::string& uri)
{
	std::string path = uri;
	size_t queryPos = path.find('?');
	if (queryPos != std::string::npos)
		path = path.substr(0, queryPos);

	size_t fragPos = path.find('#');
	if (fragPos != std::string::npos)
		path = path.substr(0, fragPos);

	return path;
}

std::string UriResolver::resolve(const HttpRequest& request, const LocationConfig*& loc, Client& Client, const ServerConfig& config)
{
	std::string path;
	std::string fullPath;

	path = extractPath(request.getUri());
	path = urlDecode(path);
	path = normalize(path);


	loc = findMatchingLocation(path);
	if (!loc)
		throw HttpException(400, "invalid path");
	if (config.keepaliveTimeoutSec == 0)
		Client.setRequestStatus(true);

	fullPath = applyRootOrAlias(path, loc);

	if (!isPathSecure(fullPath, loc))
		throw HttpException(400, "invalid path");

	return fullPath;
}
