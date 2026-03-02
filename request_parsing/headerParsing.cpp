#include "HttpParser.hpp"
#include <sstream>

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

// Tockenise and only accept "method" "path" "version" before checking
// their content. 
HttpParser::ParseResult HttpParser::parseRequestLine(const std::string& requestLine, HttpRequest& tempRequest)
{
	std::string method;
	std::string path;
	std::string version;
	ParseResult result;

	result = tockeniseRequestLine(requestLine, method, path, version);
	if (result != ParseResult::ALL_OK)
		return result;

	if (method != "GET" && method != "POST" && method != "DELETE")
		return ParseResult::INVALID_METHOD;

	if (checkPath(path))
		return ParseResult::INVALID_URI;

	if (version != "HTTP/1.1")
		return ParseResult::INVALID_HTTP_VERSION;

	tempRequest.setMethod(method);
	tempRequest.setPath(path);
	tempRequest.setVersion(version);

	return ParseResult::ALL_OK;
}

HttpParser::ParseResult HttpParser::parseHeaders(const std::string& headersBlock, HttpRequest& tempRequest)
{
	std::istringstream	stream(headersBlock);
	std::string 		line;
	std::string 		key;
	std::string 		value;
	size_t				start;
	size_t				colon;

	while (std::getline(stream, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
		    line.erase(line.size() - 1);
		if (line.empty())
			return ParseResult::INVALID_HEADER_FORMAT;
		colon = line.find(':');
		if (colon == std::string::npos)
			return ParseResult::INVALID_HEADER_FORMAT;

		key = line.substr(0, colon);
		value = line.substr(colon + 1);

		start = value.find_first_not_of(' ');
		if (start == std::string::npos)
			return ParseResult::UNEXPECTED_ERROR;
		value = value.substr(start);
		if (key.empty())
			return ParseResult::INVALID_HEADER_FORMAT;

		tempRequest.setHeader(key, value);
	}
	return ParseResult::ALL_OK;
}

// If there is a body, then retrieve and check out his size conformity.
HttpParser::ParseResult HttpParser::parseBody(const std::string& bodyPart, HttpRequest& tempRequest)
{
	unsigned long	len;
	std::string		value;
	char*			end;

    if (!tempRequest.hasHeader("content-length"))
    {
    if (!bodyPart.empty())
            return ParseResult::UNEXPECTED_BODY;

        tempRequest.setBody("");
        tempRequest.setContentLength(0);
        return ParseResult::ALL_OK;
    }

	value = tempRequest.getHeader("content-length");
	errno = 0;
	len = std::strtoul(value.c_str(), &end, 10);

	// if (len > MAX_BODY_SIZE)
	// 	return ParseResult::BODY_TOO_LARGE;    //ask where we stock maxBodySize 
	if (len > static_cast<unsigned long>(SIZE_MAX))
		return ParseResult::INVALID_CONTENT_LENGTH;
	if (errno != 0 || *end != '\0')
		return ParseResult::INVALID_CONTENT_LENGTH;
	if (bodyPart.size() < len)
        return ParseResult::INCOMPLETE;
	if (bodyPart.size() > len)
		return ParseResult::BODY_SIZE_MISMATCH;
	
    tempRequest.setContentLength(len);
    tempRequest.setBody(bodyPart.substr(0, len));

	return ParseResult::ALL_OK;
}
