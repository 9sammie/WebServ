#include "HttpParser.hpp"

HttpParser::ParseResult HttpParser::parseRequestLine(const std::string& requestLine, HttpRequest& request)
{
	size_t firstSpace = requestLine.find(' ');
	if (firstSpace == std::string::npos)
		return ParseResult::INVALID_REQUEST_LINE;
	size_t pathStart = requestLine.find_first_not_of(' ', firstSpace);
	if (pathStart == std::string::npos)
	    return ParseResult::INVALID_REQUEST_LINE;

	size_t secondSpace = requestLine.find(' ', firstSpace + 1);
	if (secondSpace == std::string::npos)
		return ParseResult::INVALID_REQUEST_LINE;
	size_t versionStart = requestLine.find_first_not_of(' ', secondSpace);
	if (versionStart == std::string::npos)
	    return ParseResult::INVALID_REQUEST_LINE;

	size_t thirdSpace = requestLine.find(' ', secondSpace + 1);
	if (thirdSpace == std::string::npos)
		return ParseResult::INVALID_REQUEST_LINE;

	std::string method = requestLine.substr(0, firstSpace);
	std::string path = requestLine.substr(firstSpace + 1, secondSpace - firstSpace - 1);
	std::string version = requestLine.substr(secondSpace + 1);

	if (version != "HTTP/1.1")
		return ParseResult::INVALID_HTTP_VERSION;

	if (method != "GET" && method != "POST" && method != "DELETE")
		return ParseResult::INVALID_METHOD;
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