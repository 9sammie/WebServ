#include "RequestHandler.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

RequestHandler::RequestHandler(const ServerConfig& config)
    : _config(config), _parser() {}

RequestHandler::RequestHandler(const RequestHandler& other)
	: _config(other._config), _parser() {}

RequestHandler& RequestHandler::operator=(const RequestHandler& other)
{
	if (this != &other)
		_config = other._config;
	return *this;
}

RequestHandler::~RequestHandler() {}

// Je travail dessus, ce n'est pas du tout finis :)
//
// std::string RequestHandler::buildHttpResponse(int statusCode, const std::string& reason,
//                                               const std::string& body,
//                                               const std::map<std::string, std::string>& extraHeaders) const
// {
//     std::ostringstream oss;
//     oss << "HTTP/1.1 " << statusCode << " " << reason << "\r\n";
//     oss << "Content-Length: " << body.size() << "\r\n";
//     oss << "Connection: close\r\n";
//     // entêtes supplémentaires (Content-Type, Location, etc.)
//     for (std::map<std::string, std::string>::const_iterator it = extraHeaders.begin();
//          it != extraHeaders.end(); ++it)
//     {
//         oss << it->first << ": " << it->second << "\r\n";
//     }
//     oss << "\r\n";
//     oss << body;
//     return oss.str();
// }

std::string RequestHandler::handleRequest(std::string& buffer)
{
    HttpRequest request;
	std::string fullPath;

    try
    {
        _parser.parseRequest(buffer, request);
    }
    catch (const HttpException& he)
    {
        int code = he.getStatusCode();
        if (code == 203)
            return std::string(); // attente de données supplémentaires

        int status = 400;
        std::string reason = "Bad Request";
        std::string body = "<html><body><h1>" + std::to_string(status) + " " + reason + "</h1></body></html>";
        return buildHttpResponse(status, reason, body);
    }
    catch (...)
    {
        // erreur imprévue côté parser -> 500
        return buildHttpResponse(500, "Internal Server Error",
                                 "<html><body><h1>500 Internal Server Error</h1></body></html>");
    }



	try
    {
        UriResolver resolver(_config);
        fullPath = resolver.resolve(request);
    }
    catch (const HttpException& he)
    {
        int code = he.getStatusCode();
        std::string reason = (code == 404 ? "Not Found" : "Internal Server Error");
        std::string body = "<html><body><h1>" + std::to_string(code) + " " + reason + "</h1></body></html>";
        return buildHttpResponse(code, reason, body);
    }
    catch (...)
    {
        return buildHttpResponse(500, "Internal Server Error",
                                 "<html><body><h1>500 Internal Server Error</h1></body></html>");
    }

    if (fullPath.empty())
    {
        return buildHttpResponse(404, "Not Found",
                                 "<html><body><h1>404 Not Found</h1></body></html>");
    }
}
