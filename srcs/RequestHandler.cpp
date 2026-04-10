#include "RequestHandler.hpp"
#include <fstream>
#include <unistd.h>

RequestHandler::RequestHandler(const ServerConfig& config)
    : _config(config), _parser(),_closeConnection(false)
{
    _methodHandlers["GET"] = &RequestHandler::handleGET;
    _methodHandlers["POST"] = &RequestHandler::handlePOST;
    _methodHandlers["DELETE"] = &RequestHandler::handleDELETE;
}

RequestHandler::RequestHandler(const RequestHandler& other)
	: _methodHandlers(other._methodHandlers), _config(other._config), _parser(), _closeConnection(false) {}

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
			return "CGI-STARTED";
		}
		Client.setRequestStatus(true);
		return buildStatusResponse(500);
	}
	return "";
}

std::string RequestHandler::executeMethodHandler(Client& Client, const HttpRequest& request, const std::string& fullPath, const LocationConfig* loc)
{
	(void)Client;
	std::map<std::string, MethodHandler>::const_iterator it;

	it = _methodHandlers.find(request.getMethod());

	if (it == _methodHandlers.end())
		return buildStatusResponse(405);

	return (this->*(it->second))(request, fullPath, loc);
}

// here we get the complete request, parse it, execute it, then return an
// appropriate response to the serverManager.
//
// The complex part here is the iteration "it" of the map<string methods
// functions>, wich will find the right method string so we can use the
// function assiociated with this string (it->second).
#include <stdio.h>
std::string RequestHandler::handleRequest(Client& Client)
{
    HttpRequest request;
    std::string fullPath;
	std::string response;
	const LocationConfig* loc = NULL;

	if (Client.getRequestStatus() == true)
		return buildStatusResponse(400);
	
	if (!(response = validateParsing(Client, request)).empty())
		return response;

	if (!(response = validateLocation(Client, request, loc, fullPath)).empty())
		return response;

	if (!(response = handleCgiExecution(Client, request, loc, fullPath)).empty())
		return (response == "CGI-STARTED" ? "" : response);

    response = executeMethodHandler(Client, request, fullPath, loc);

	return updateCloseStatus(Client, response);
}
