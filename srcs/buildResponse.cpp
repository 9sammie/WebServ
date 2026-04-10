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

static std::string getStatusMessage(int code)
{
	if (code == 200) return "OK";
	if (code == 201) return "Created";
	if (code == 204) return "No Content";
	if (code == 400) return "Bad Request";
	if (code == 403) return "Ressource Access Forbidden";
	if (code == 404) return "Ressource Not Found";
	if (code == 405) return "Method Not Allowed On This Location";
	if (code == 408) return "Client Too Slow";
	if (code == 411) return "Body Without Content Length";
	if (code == 413) return "Body Too Large";
	if (code == 414) return "URI Too Long";
	if (code == 500) return "Internal Server Error";
	if (code == 502) return "Cgi Problem";
	if (code == 505) return "Wrong HTTP Version";
	return "Internal Server Error";
}

std::string RequestHandler::buildStatusResponse(int code) const
{
	if (code >= 200 && code < 300)
		return buildHttpResponse(code, "No Content", "", false);
	std::string message = getStatusMessage(code);

	std::ostringstream body;
	body << "<html><body><h1>" << code << " " << message << "</h1></body></html>";
	
	std::map<std::string, std::string> headers;
	if (!mimeType.empty())
		headers["Content-Type"] = mimeType;
	else
		headers["Content-Type"] = "text/html";


	return buildHttpResponse(code, message, body.str(), true, headers);
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
                                               const std::map<std::string, std::string>& extraHeaders)
{
	std::ostringstream response;
	response << "HTTP/1.1 " << statusCode << " " << reason << "\r\n";

	response << "Server: Rats_du_port_80\r\n";
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
