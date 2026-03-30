#include "UriResolver.hpp"
#include "HttpRequest.hpp"
#include <cctype>
#include <sstream>
#include <climits>
#include <cstdlib>

// Uniform Ressource Identifier
UriResolver::UriResolver(const ServerConfig& config): _config(config) {}

UriResolver::~UriResolver() {}



bool UriResolver::isPathSecure(const std::string& fullPath)
{
	char actualPath[PATH_MAX];

	if (realpath(fullPath.c_str(), actualPath) == NULL)
		return true; 

	std::string realFullPath(actualPath);

	char rootAbs[PATH_MAX];
	if (realpath(_config.root.c_str(), rootAbs) == NULL)
		return false;

	std::string realRoot(rootAbs);

	if (realFullPath.compare(0, realRoot.size(), realRoot) == 0)
		return true;

	return false;
}

std::string UriResolver::applyRootOrAlias(const std::string& path, const LocationConfig* loc)
{
std::string base;
    std::string remaining;

    if (loc)
    {
        base = loc->root;
        std::string prefix = loc->prefix;

        if (path.compare(0, prefix.size(), prefix) == 0)
        {
            remaining = path.substr(prefix.size());
        }
        else
        {
            remaining = path;
        }
    }
    else
    {
        base = _config.root;
        remaining = path;
    }

    std::string fullPath = base + "/" + remaining;

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

	printf("--- DEBUG MATCHING pour : %s ---\n", path.c_str());		
    for (size_t i = 0; i < locations.size(); ++i)
    {
        const std::string& prefix = locations[i].prefix;
        printf("  Check prefix: [%s] \n", prefix.c_str());

        if (path.compare(0, prefix.size(), prefix) == 0)
        {
            if (path.size() == prefix.size() || prefix[prefix.size() - 1] == '/' || path[prefix.size()] == '/')
            {
				printf(" -> MATCH\n");
                if (prefix.size() >= longestLength)
                {
                    longestLength = prefix.size();
                    bestMatch = &locations[i];
					printf("NOUBEAU BEST\n");
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

// Ici on check s'il y a des caracteres encodé et on les remplacent par leur valeur en ascii (ex: )
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

std::string UriResolver::resolve(const HttpRequest& request, const LocationConfig*& loc)
{
	std::string path;
	std::string fullPath;

	path = extractPath(request.getUri());
	path = urlDecode(path);
	path = normalize(path);
	loc = findMatchingLocation(path);
	fullPath = applyRootOrAlias(path, loc);
	if (!isPathSecure(fullPath))
		throw HttpException(203, "invalid path");
	printf("77777777777777777 fullPath:     %s", fullPath.c_str());
	return fullPath;
}
