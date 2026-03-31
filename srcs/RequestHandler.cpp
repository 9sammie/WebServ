#include "RequestHandler.hpp"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cctype>

RequestHandler::RequestHandler(const ServerConfig& config)
    : _config(config), _parser(), _closeConnection(false) {}

RequestHandler::RequestHandler(const RequestHandler& other)
	: _config(other._config), _parser(), _closeConnection(false) {}

RequestHandler::~RequestHandler() {}



static std::string getReasonPhrase(int code)
{
	if (code == 200) return "OK";
	if (code == 201) return "Created";
	if (code == 204) return "No Content";
	if (code == 400) return "Bad Request";
	if (code == 403) return "Forbidden";
	if (code == 404) return "Not Found";
	if (code == 405) return "Method Not Allowed";
	if (code == 500) return "Internal Server Error";
	return "Internal Server Error";
}

std::string RequestHandler::buildStatusResponse(int code) const
{
	if (code >= 200 && code < 300)
		return buildHttpResponse(code, "No Content", "", false);
	std::string reason = getReasonPhrase(code);
	std::ostringstream oss;
	oss << "<html><body><h1>" << code << " " << reason << "</h1></body></html>";
	return buildHttpResponse(code, reason, oss.str(), true);
}

std::string RequestHandler::buildHttpResponse(int statusCode,
                                               const std::string& reason,
                                               const std::string& body,
											   const bool closeConnection,
                                               const std::map<std::string, std::string>& extraHeaders)
{
	std::ostringstream oss;
	oss << "HTTP/1.1 " << statusCode << " " << reason << "\r\n";
	if (closeConnection == true)
		oss << "Connection: close\r\n";

    if (!body.empty())
    {
        oss << "Content-Length: " << body.size() << "\r\n";
        if (extraHeaders.find("Content-Type") == extraHeaders.end())
            oss << "Content-Type: text/html\r\n";
    }

	for (std::map<std::string,std::string>::const_iterator it = extraHeaders.begin();
		it != extraHeaders.end(); ++it)
	{
		oss << it->first << ": " << it->second << "\r\n";
	}

	oss << "\r\n";
	oss << body;
	return oss.str();
}

std::string RequestHandler::handleRequest(Client& Client)
{
    HttpRequest request;
    std::string fullPath;
	const LocationConfig* loc = NULL;
	DataCgi data;

    try
    {
        _parser.parseRequest(Client.getBuffer(Client::REQUEST), request, _config);
    }
    catch (const HttpException& he)
    {
        int code = he.getStatusCode();
        if (code == 203)
            return "";
		Client.setCloseStatus(true);
		return buildStatusResponse(code);
    }

    try
    {
        UriResolver locateRessource(_config);
        fullPath = locateRessource.resolve(request, loc, Client);
    }
    catch (const HttpException& he)
    {
        int code = he.getStatusCode();
		if (code < 400)
			return buildHttpResponse(500, "Internal Server Error", "<html><body><h1>500 Internal Server Error</h1></body></html>", true);
		Client.setCloseStatus(true);
		return buildStatusResponse(code);
    }

    if (fullPath.empty())
	{
        return buildStatusResponse(404);
	}



	if (loc && !loc->cgiPath.empty() && isCgiRequest(fullPath, loc))
	{
		DataCgi data = fillCgiData(request, fullPath, loc, Client);
		Client::CgiInfo cgi = CgiHandler(data);

		if (cgi.isCgi)
		{
			Client.setCgiInfo(cgi); 
			return "CGI_STARTED";
		}
		return buildStatusResponse(500);
	}

    const std::string& method = request.getMethod();

    if (method == "GET")
        return handleGET(request, fullPath, loc);

    if (method == "POST")
        return handlePOST(request, fullPath);

    if (method == "DELETE")
        return handleDELETE(fullPath);



    return buildStatusResponse(405);
}
