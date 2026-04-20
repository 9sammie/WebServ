#include "RequestHandler.hpp"
#include <ctime>


std::string RequestHandler::updateCloseStatus(Client& client, const std::string& response)
{
	if (!response.empty())
	{
        int code = extractStatusCode(response);

        if (code >= 300 || code == 0)
			client.setCloseStatus(true);
    }
    return response;
}

int RequestHandler::extractStatusCode(const std::string& response) const
{
	if (response.size() < 12) 
		return 0;

	std::string codeStr = response.substr(9, 3);
	return std::atoi(codeStr.c_str());
}

std::string RequestHandler::getStatusMessage(const int code) const
{
	switch(code)
	{
		case 200: return "Ok";
		case 201: return "Created";
		case 204: return "No Content";
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 400: return "Bad Request";
		case 403: return "Ressource Access Forbidden";
		case 404: return "Ressource Not Found";
		case 405: return "Method Not Allowed On This Location";
		case 408: return "Client Too Slow";
		case 411: return "Body Without Content Length";
		case 413: return "Payload Too Large";
		case 414: return "URI Too Long";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 504: return "Gateway Timeout";
		case 505: return "Wrong HTTP Version";

		default: return "Internal Server Error";
	}
}

std::string RequestHandler::buildStatusResponse(int code, Client& Client)
{
	if (code >= 200 && code < 300 && Client.getCloseStatus() == false)
		return buildHttpResponse(code, "No Content", "", false, _config.serverName);
	if (code >= 200 && code < 300 && Client.getCloseStatus() == true)
		return buildHttpResponse(code, "No Content", "", true, _config.serverName);

	std::string message = getStatusMessage(code);

	std::ostringstream body;
	body << "<html><body><h1>" << code << " " << message << "</h1></body></html>";

	std::map<std::string, std::string> headers;
	if (!mimeType.empty())
		headers["Content-Type"] = mimeType;
	else
		headers["Content-Type"] = "text/html";

	return buildHttpResponse(code, message, body.str(), true, _config.serverName, headers);
}

std::string getRfc7231Date()
{
	char buf[100];
	std::time_t now = std::time(0);
	struct std::tm *tm = std::gmtime(&now);
	std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", tm);
	return std::string(buf);
}

std::string RequestHandler::buildHttpResponse(int statusCode,
                                               const std::string& reason,
                                               const std::string& body,
											   const bool closeConnection,
											   const std::string serverName,
                                               const std::map<std::string, std::string>& extraHeaders)
{
	std::ostringstream response;
	response << "HTTP/1.1 " << statusCode << " " << reason << "\r\n";

	response << "Server: " << _config.serverName << "\r\n";
	response << "Date: " << getRfc7231Date() << "\r\n";

	if (closeConnection == true)
		response << "Connection: close\r\n";
	else
		response << "Connection: keep-alive\r\n";

	response << "Content-Length: " << body.size() << "\r\n";

	if (!body.empty())
	{
		std::map<std::string, std::string>::const_iterator it = extraHeaders.find("Content-Type");

		if (it != extraHeaders.end())
			response << "Content-Type: " << it->second << "\r\n";
		else
			response << "Content-Type: text/plain\r\n";
	}

	for (std::map<std::string,std::string>::const_iterator it = extraHeaders.begin();
		it != extraHeaders.end(); ++it)
	{
		if (it->first != "Content-Type")
			response << it->first << ": " << it->second << "\r\n";
	}

	response << "\r\n";
	response << body;
	return response.str();
}
