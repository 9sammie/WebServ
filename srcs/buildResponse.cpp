#include "RequestHandler.hpp"

int RequestHandler::extractStatusCode(const std::string& response) const
{
	if (response.size() < 12) 
		return 0;

	std::string codeStr = response.substr(9, 3);
	return std::atoi(codeStr.c_str());
}

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

	oss << "Content-Length: " << body.size() << "\r\n";

    if (!body.empty())
    {
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
