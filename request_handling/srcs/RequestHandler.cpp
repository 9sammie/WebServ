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

// Je travail dessus, ce n'est pas du tout finis :)

std::string RequestHandler::buildHttpResponse(
    int statusCode,
    const std::string& reason,
    const std::string& body,
    const std::map<std::string,std::string>& extraHeaders) const
{
    std::ostringstream oss;

    oss << "HTTP/1.1 " << statusCode << " " << reason << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n";

    for (std::map<std::string,std::string>::const_iterator it = extraHeaders.begin();
         it != extraHeaders.end(); ++it)
    {
        oss << it->first << ": " << it->second << "\r\n";
    }

    oss << "\r\n";
    oss << body;

    return oss.str();
}

std::string RequestHandler::handleDELETE(const std::string& path)
{
    if (std::remove(path.c_str()) != 0)
    {
        return buildHttpResponse(404, "Not Found",
            "<html><body><h1>404 Not Found</h1></body></html>");
    }

    return buildHttpResponse(200, "OK",
        "<html><body><h1>200 File deleted</h1></body></html>");
}

std::string RequestHandler::handlePOST(const HttpRequest& request, const std::string& path)
{
    std::ofstream file(path.c_str(), std::ios::binary);

    if (!file)
    {
        return buildHttpResponse(500, "Internal Server Error",
            "<html><body><h1>500 Internal Server Error</h1></body></html>");
    }

    file << request.getBody();

    return buildHttpResponse(201, "Created",
        "<html><body><h1>201 Created</h1></body></html>");
}

std::string RequestHandler::handleGET(const std::string& path)
{
    std::ifstream file(path.c_str(), std::ios::binary);

    if (!file)
    {
        return buildHttpResponse(404, "Not Found",
            "<html><body><h1>404 Not Found</h1></body></html>");
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    std::string body = buffer.str();

    return buildHttpResponse(200, "OK", body,
        {{"Content-Type", "text/html"}});
}

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
            return "";

        return buildHttpResponse(400, "Bad Request",
            "<html><body><h1>400 Bad Request</h1></body></html>");
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

        return buildHttpResponse(code, reason,
            "<html><body><h1>" + std::to_string(code) + " " + reason + "</h1></body></html>");
    }

    if (fullPath.empty())
    {
        return buildHttpResponse(404, "Not Found",
            "<html><body><h1>404 Not Found</h1></body></html>");
    }

    const std::string& method = request.getMethod();

    if (method == "GET")
        return handleGET(fullPath);

    if (method == "POST")
        return handlePOST(request, fullPath);

    if (method == "DELETE")
        return handleDELETE(fullPath);

    return buildHttpResponse(405, "Method Not Allowed",
        "<html><body><h1>405 Method Not Allowed</h1></body></html>");
}
