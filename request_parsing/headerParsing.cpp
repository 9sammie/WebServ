#include "HttpParser.hpp"

bool	checkPath(std::string path)
{
	if (path.empty() || path[0] != '/' || path.find(' ') != std::string::npos || path.find("..") != std::string::npos)
		return 1;

	for (size_t i = 0; i < path.size(); ++i)
	{
	    unsigned char c = path[i];
	    if (std::iscntrl(c))
	        return 1;
	}

	return 0;
}

HttpParser::ParseResult HttpParser::parseRequestLine(const std::string& requestLine, HttpRequest& request)
{
	size_t firstSpace;
	size_t pathStart;
	size_t secondSpace;
	size_t versionStart;
	size_t versionEnd;
	std::string method;
	std::string path;
	std::string version;

	firstSpace = requestLine.find(' ');
	if (firstSpace == std::string::npos)
		return ParseResult::INVALID_REQUEST_LINE;
	method = requestLine.substr(0, firstSpace);

	pathStart = requestLine.find_first_not_of(' ', firstSpace);
	if (pathStart == std::string::npos)
	    return ParseResult::INVALID_REQUEST_LINE;
	secondSpace = requestLine.find(' ', firstSpace + 1);
	if (secondSpace == std::string::npos)
		return ParseResult::INVALID_REQUEST_LINE;
	path = requestLine.substr(pathStart, secondSpace - pathStart);

	versionStart = requestLine.find_first_not_of(' ', secondSpace);
	if (versionStart == std::string::npos)
	    return ParseResult::INVALID_REQUEST_LINE;
	versionEnd = requestLine.find(' ', versionStart);
	if (versionEnd == std::string::npos)
	    version = requestLine.substr(versionStart);
	else
	{
		if (requestLine.find_first_not_of(' ', versionEnd) != std::string::npos)
			return ParseResult::INVALID_REQUEST_LINE;
	    version = requestLine.substr(versionStart, versionEnd - versionStart);
	}

	if (method != "GET" && method != "POST" && method != "DELETE")
		return ParseResult::INVALID_METHOD;
	
	if (checkPath(path))
		return ParseResult::INVALID_URI;
	
	if (version != "HTTP/1.1")
		return ParseResult::INVALID_HTTP_VERSION;

	request.setMethod(method);
	request.setPath(path);
	request.setVersion(version);

	return ParseResult::ALL_OK;
}

HttpParser::ParseResult HttpParser::parseHeader(const std::string& headerLines, HttpRequest& request)
{
	return ParseResult::ALL_OK;
}

// If there is a body, then retrieve and check out his size conformity.
HttpParser::ParseResult HttpParser::parseBody(const std::string& bodyPart, HttpRequest& request)
{
    if (request.hasHeader("Content-Length"))
    {
        request.setContentLength(std::atoi(request.getHeader("Content-Length").c_str()));
		if (bodyPart.size() < request.getContentLength())
			return ParseResult::INCOMPLETE;
        else if (bodyPart.size() != request.getContentLength())
            return ParseResult::BODY_SIZE_MISMATCH;
    }

    request.setBody(bodyPart);
	return ParseResult::ALL_OK;
}