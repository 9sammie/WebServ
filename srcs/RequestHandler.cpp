#include "RequestHandler.hpp"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cctype>

RequestHandler::RequestHandler(const ServerConfig& config)
    : _config(config), _parser() {}

RequestHandler::RequestHandler(const RequestHandler& other)
	: _config(other._config), _parser() {}

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
	if (code == 204)
		return buildHttpResponse(204, "No Content", "");
	std::string reason = getReasonPhrase(code);
	std::ostringstream oss;
	oss << "<html><body><h1>" << code << " " << reason << "</h1></body></html>";
	return buildHttpResponse(code, reason, oss.str());
}

// Je travail dessus, ce n'est pas du tout finis :)

std::string RequestHandler::buildHttpResponse(int statusCode,
                                               const std::string& reason,
                                               const std::string& body,
                                               const std::map<std::string, std::string>& extraHeaders) const
{
	std::ostringstream oss;
	oss << "HTTP/1.1 " << statusCode << " " << reason << "\r\n";
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

    try
    {
        _parser.parseRequest(Client.getBuffer(Client::REQUEST), request);
    }
    catch (const HttpException& he)
    {
        int code = he.getStatusCode();
        if (code == 203)
            return "";
		return buildStatusResponse(code);
    }

    try
    {
        UriResolver locateRessource(_config);
        fullPath = locateRessource.resolve(request, loc);
    }
    catch (const HttpException& he)
    {
        int code = he.getStatusCode();
		if (code < 400)
			return buildHttpResponse(500, "Internal Server Error", "<html><body><h1>500 Internal Server Error</h1></body></html>");
		return buildStatusResponse(code);
    }

    if (fullPath.empty())
        return buildStatusResponse(404);

    const std::string& method = request.getMethod();

    if (method == "GET")
        return handleGET(request, fullPath, loc);

    if (method == "POST")
        return handlePOST(request, fullPath);

    if (method == "DELETE")
        return handleDELETE(fullPath);

    return buildStatusResponse(405);
}
