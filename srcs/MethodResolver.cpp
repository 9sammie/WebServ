#include "RequestHandler.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <dirent.h>

std::string RequestHandler::handleDELETE(const HttpRequest& request, const std::string& path, const LocationConfig* loc, Client& Client)
{
	(void)request;

	std::string toFind = "DELETE";
	std::vector<std::string>::const_iterator it = std::find(loc->methods.begin(), loc->methods.end(), toFind);

	if (it == loc->methods.end())
		return buildStatusResponse(405, Client);

    struct stat st;
    if (stat(path.c_str(), &st) != 0)
        return buildStatusResponse(404, Client);

    if (S_ISDIR(st.st_mode))
        return buildStatusResponse(405, Client);

    std::string parentDir = path.substr(0, path.rfind('/'));
    if (access(parentDir.c_str(), W_OK) != 0)
        return buildStatusResponse(403, Client);

    if (std::remove(path.c_str()) != 0)
        return buildStatusResponse(500, Client);

    return buildStatusResponse(204, Client);
}

std::string RequestHandler::handlePOST(const HttpRequest& request, const std::string& path, const LocationConfig* loc, Client& Client)
{
	std::string toFind = "POST";
	std::vector<std::string>::const_iterator it = std::find(loc->methods.begin(), loc->methods.end(), toFind);

	if (it == loc->methods.end())
		return buildStatusResponse(405, Client);

	std::string parentDir = path.substr(0, path.rfind('/'));
	struct stat st;

	if (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
		return buildStatusResponse(404, Client);

	if (stat(parentDir.c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
		return buildStatusResponse(403, Client);

	if (access(parentDir.c_str(), W_OK) != 0)
		return buildStatusResponse(403, Client);

	bool fileExisted = (access(path.c_str(), F_OK) == 0);

	std::ofstream file(path.c_str(), std::ios::binary);
	if (!file)
    	return buildStatusResponse(500, Client);

	file << request.getBody();
	if (file.fail())
    	return buildStatusResponse(500, Client);

	if (!request.hasHeader("content-length"))
    	return buildStatusResponse(411, Client);

	return buildStatusResponse(fileExisted ? 200 : 201, Client);
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

bool RequestHandler::resolvePath(const HttpRequest& request, const std::string& path, const LocationConfig* loc, std::string& outPath, std::string& outResponse, Client& Client)
{
	if (loc && loc->hasRedirection)
	{
		std::map<std::string, std::string> headers;
		headers["location"] = loc->redirectTarget;
		outResponse = buildHttpResponse(loc->redirectCode, getStatusMessage(loc->redirectCode), "", Client.getCloseStatus(), _config.serverName, headers);
		return false;
	}

    struct stat st;
    if (stat(path.c_str(), &st) != 0)
	{
        outResponse = buildStatusResponse(404, Client);
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
				outResponse = buildStatusResponse(500, Client);
				return false;
			}
			std::map<std::string, std::string> headers;
			headers["Content-Type"] = "text/html";
			outResponse = buildHttpResponse(200, "OK", listing, Client.getCloseStatus(), _config.serverName, headers);
			return false;
		}
		outResponse = buildStatusResponse(403, Client);
		return false;
	}

	outPath = path;
	return true;
}

std::string RequestHandler::handleGET(const HttpRequest& request, const std::string& path, const LocationConfig* loc, Client& Client)
{
	std::string resolvedPath;
	struct stat st;
	std::ostringstream buffer;
	std::map<std::string, std::string>	headers;
	std::string	body;
	std::string earlyResponse;

	if (std::find(loc->methods.begin(), loc->methods.end(), "GET") == loc->methods.end())
		return buildStatusResponse(405, Client);

	if (request.getUri().find("/set-cursor") != std::string::npos) 
	{
		std::map<std::string, std::string> headers;
		headers["set-cookie"] = "mouse_type=" + request.getQueryParam("type") + "; Path=/; Max-Age=3600";
		headers["location"] = "/home.html"; 
		return buildHttpResponse(302, "Found", "", Client.getCloseStatus(), _config.serverName, headers);
	}

	if(!resolvePath(request, path, loc, resolvedPath, earlyResponse, Client))
		return earlyResponse;
	
	if (access(resolvedPath.c_str(), F_OK) != 0)
		return buildStatusResponse(404, Client);

	if (access(resolvedPath.c_str(), R_OK) != 0)
		return buildStatusResponse(403, Client);

	if (stat(resolvedPath.c_str(), &st) != 0)
		return buildStatusResponse(500, Client);

	if (!S_ISREG(st.st_mode))
		return buildStatusResponse(403, Client);

	if (access(resolvedPath.c_str(), R_OK) != 0)
		return buildStatusResponse(403, Client);

	std::ifstream file(resolvedPath.c_str(), std::ios::binary);
	if (!file)
		return buildStatusResponse(500, Client);
	
	buffer << file.rdbuf();
	body = buffer.str();
	mimeType = getMimeType(resolvedPath);

	if (mimeType == "text/html") 
		applyHtmlTemplates(body, request);

	headers["Content-Type"] = mimeType;
	return buildHttpResponse(200, "OK", body, Client.getCloseStatus(), _config.serverName, headers);
}
