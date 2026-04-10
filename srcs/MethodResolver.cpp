#include "RequestHandler.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <dirent.h>

std::string RequestHandler::handleDELETE(const HttpRequest& request, const std::string& path, const LocationConfig* loc)
{
	(void)request;

	std::string toFind = "DELETE";
	std::vector<std::string>::const_iterator it = std::find(loc->methods.begin(), loc->methods.end(), toFind);

	if (it == loc->methods.end())
		return buildStatusResponse(405);

    struct stat st;
    if (stat(path.c_str(), &st) != 0)
        return buildStatusResponse(404);

    if (S_ISDIR(st.st_mode))
        return buildStatusResponse(405);

    std::string parentDir = path.substr(0, path.rfind('/'));
    if (access(parentDir.c_str(), W_OK) != 0)
        return buildStatusResponse(403);

    if (std::remove(path.c_str()) != 0)
        return buildStatusResponse(500);

    return buildStatusResponse(204);
}

std::string RequestHandler::handlePOST(const HttpRequest& request, const std::string& path, const LocationConfig* loc)
{
	std::string toFind = "POST";
	std::vector<std::string>::const_iterator it = std::find(loc->methods.begin(), loc->methods.end(), toFind);

	if (it == loc->methods.end())
		return buildStatusResponse(405);

	std::string parentDir = path.substr(0, path.rfind('/'));
	struct stat st;

	if (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
		return buildStatusResponse(404);

	if (stat(parentDir.c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
		return buildStatusResponse(403);

	if (access(parentDir.c_str(), W_OK) != 0)
		return buildStatusResponse(403);

	bool fileExisted = (access(path.c_str(), F_OK) == 0);

	std::ofstream file(path.c_str(), std::ios::binary);
	if (!file)
    	return buildStatusResponse(500);

	file << request.getBody();
	if (file.fail())
    	return buildStatusResponse(500);

	return buildStatusResponse(fileExisted ? 200 : 201);
}

std::string RequestHandler::getMimeType(const std::string& path)
{
    size_t dot = path.rfind('.');
    if (dot == std::string::npos)
        return "application/octet-stream";

    std::string ext = path.substr(dot + 1);

    if (ext == "html" || ext == "htm") return "text/html";
    if (ext == "css")                  return "text/css";
    if (ext == "js")                   return "text/javascript";
    if (ext == "json")                 return "application/json";
    if (ext == "png")                  return "image/png";
    if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
    if (ext == "gif")                  return "image/gif";
    if (ext == "ico")                  return "image/x-icon";
    if (ext == "pdf")                  return "application/pdf";
    if (ext == "txt")                  return "text/plain";

    return "application/octet-stream";
}

// génère une page HTML listant le contenu d'un dossier
static std::string buildDirectoryListing(const std::string& uriPath, const std::string& fsPath)
{
    std::ostringstream html;
    html << "<html><body><h1>Index of " << uriPath << "</h1><ul>";

    DIR* dir = opendir(fsPath.c_str());
    if (!dir)
        return "";

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        html << "<li><a href=\"" << uriPath;
        if (uriPath.empty() || uriPath[uriPath.size() - 1] != '/')
            html << "/";
        html << name << "\">" << name << "</a></li>";
    }
    closedir(dir);

    html << "</ul></body></html>";
    return html.str();
}

bool RequestHandler::resolvePath(const HttpRequest& request, const std::string& path, const LocationConfig* loc, std::string& outPath, std::string& outResponse)
{
    struct stat st;
    if (stat(path.c_str(), &st) != 0)
    {
        outResponse = buildStatusResponse(404);
        return false;
    }

    if (S_ISDIR(st.st_mode))
    {
		const std::string& defaultFile = loc->index;
		if (!defaultFile.empty())
		{
			std::string indexPath = path;
			if (indexPath[indexPath.size() - 1] != '/')
				indexPath += "/";
			indexPath += defaultFile;
			struct stat ist;
			if (stat(indexPath.c_str(), &ist) == 0 && S_ISREG(ist.st_mode))
			{
				outPath = indexPath;
				return true;
			}
		}
		if (loc->autoindex)
		{
			std::string listing = buildDirectoryListing(request.getUri(), path);
			if (listing.empty())
			{
				outResponse = buildStatusResponse(500);
				return false;
			}
			std::map<std::string, std::string> headers;
			headers["Content-Type"] = "text/html";
			outResponse = buildHttpResponse(200, "OK", listing, false, headers);
			return false;
		}
		outResponse = buildStatusResponse(403);
		return false;
	}

	outPath = path;
	return true;
}

std::string RequestHandler::handleGET(const HttpRequest& request, const std::string& path, const LocationConfig* loc)
{
	std::string toFind = "GET";
	std::string resolvedPath;
	struct stat st;
	std::ostringstream buffer;
	std::map<std::string, std::string>	headers;
	std::string	body;
	std::string earlyResponse;
	std::vector<std::string>::const_iterator it = std::find(loc->methods.begin(), loc->methods.end(), toFind);

	if (request.getUri().find("/set-cursor") != std::string::npos) 
	{
		std::string type = request.getQueryParam("type");
		std::map<std::string, std::string> headers;

		headers["set-cookie"] = "mouse_type=" + type + "; Path=/; Max-Age=3600";
		headers["location"] = "/home.html"; 

		return buildHttpResponse(302, "Found", "", false, headers);
	}

	if (request.getMethod() == "GET")
	{
		if (access(path.c_str(), R_OK) != 0)
			return buildStatusResponse(403);
	}

	if (it == loc->methods.end())
		return buildStatusResponse(405);
	if(!resolvePath(request, path, loc, resolvedPath, earlyResponse))
		return earlyResponse;

	if (stat(resolvedPath.c_str(), &st) != 0)
		return buildStatusResponse(404);

	if (!S_ISREG(st.st_mode))
		return buildStatusResponse(403);

	if (access(resolvedPath.c_str(), R_OK) != 0)
		return buildStatusResponse(403);

	std::ifstream file(resolvedPath.c_str(), std::ios::binary);
	if (!file)
		return buildStatusResponse(500);	
	buffer << file.rdbuf();
	body = buffer.str();

	mimeType = getMimeType(resolvedPath);
	if (mimeType == "text/html") 
		applyHtmlTemplates(body, request);

	headers["Content-Type"] = mimeType;
	return buildHttpResponse(200, "OK", body, false, headers);
}
