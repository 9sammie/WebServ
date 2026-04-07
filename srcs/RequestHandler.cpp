#include "RequestHandler.hpp"
#include <fstream>

RequestHandler::RequestHandler(const ServerConfig& config)
    : _config(config), _parser(),_closeConnection(false)
{
    _methodHandlers["GET"] = &RequestHandler::handleGET;
    _methodHandlers["POST"] = &RequestHandler::handlePOST;
    _methodHandlers["DELETE"] = &RequestHandler::handleDELETE;
}

RequestHandler::RequestHandler(const RequestHandler& other)
	: _config(other._config), _parser(), _closeConnection(false), _methodHandlers(other._methodHandlers) {}

RequestHandler::~RequestHandler() {}



std::string	RequestHandler::handleCgiExecution(Client& Client, HttpRequest& request, const LocationConfig* loc, std::string& fullPath)
{
	DataCgi data;
	Client::CgiInfo cgi;

	if (loc && !loc->cgiPath.empty() && isCgiRequest(fullPath, loc))
	{
		data = fillCgiData(request, fullPath, loc, Client);
		cgi = CgiHandler(data);

		if (cgi.isCgi)
		{
			Client.setCgiInfo(cgi);
			return "CGI_STARTED";
		}
		return buildStatusResponse(500);
	}
	return "";
}

// here we get the complete request, parse it, execute it, then return an
// appropriate response to the serverManager.
//
// The complex part here is the iteration "it" of the map<string methods
// functions>, wich will find the right method string so we can use the
// function assiociated with this string (it->second).
std::string RequestHandler::handleRequest(Client& Client)
{
    HttpRequest request;
    std::string fullPath;
	std::string response;
	const LocationConfig* loc = NULL;
	std::map<std::string, MethodHandler>::const_iterator it;

	printf("buffer: %s\n", Client.getBuffer(Client::REQUEST).c_str());
	if (!(response = validateParsing(Client, request)).empty())
		return response;

	if (!(response = validateLocation(Client, request, loc, fullPath)).empty())
		return response;

	if (!(response = handleCgiExecution(Client, request, loc, fullPath)).empty())
		return "";

	it = _methodHandlers.find(request.getMethod());
	if (it == _methodHandlers.end())
		return buildStatusResponse(405);

    response = (this->*(it->second))(request, fullPath, loc);
    if (!response.empty())
	{
        int code = extractStatusCode(response);
        if (code >= 300 || code == 0) Client.setCloseStatus(true);
    }
    return response;
}
