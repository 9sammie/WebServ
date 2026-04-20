#include "RequestHandler.hpp"

std::string RequestHandler::validateParsing(Client& Client, HttpRequest& request)
{
	try
	{
		_parser.parseRequest(Client.getBuffer(Client::REQUEST), request, _config);
	}
	catch (const HttpException& he)
	{
		int code = he.getStatusCode();
		Client.setCloseStatus(true);
		return buildStatusResponse(code, Client);
	}
	
	std::string status = "close";
	if(request.hasHeader("connection") == 1 && request.getHeader("connection") == status)
		Client.setCloseStatus(true);

	return "";
}

// Here we find the right request location, save it, then check if it exists and if there is a redirection.
std::string RequestHandler::validateLocation(Client& Client, HttpRequest& request, const LocationConfig*& loc, std::string& fullPath)
{
	try
	{
		UriResolver locateRessource(_config);
		fullPath = locateRessource.resolve(request, loc, Client, _config);
	}
	catch (const HttpException& he)
	{
		int code = he.getStatusCode();
		if (code < 400)
			return buildHttpResponse(500, "Internal Server Error", "<html><body><h1>500 Internal Server Error</h1></body></html>", true, _config.serverName);
		Client.setCloseStatus(true);
		return buildStatusResponse(code, Client);
	}
    if (fullPath.empty() && !loc->hasRedirection)
	{
        return buildStatusResponse(404, Client);
	}

	return "";
}
