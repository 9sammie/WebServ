#include "HttpParser.hpp"

HttpParser::HttpParser() {}

HttpParser::HttpParser(const HttpParser& other) {}

HttpParser& HttpParser::operator=(const HttpParser& other) {}

HttpParser::~HttpParser() {}

HttpParser::ParseResult HttpParser::getRequestParts(std::string& buffer, std::string& requestLine, std::string& headerLines, std::string& bodyPart)
{
	size_t headerEnd = buffer.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return ParseResult::INCOMPLETE;

    std::string headerPart = buffer.substr(0, headerEnd);
    bodyPart = buffer.substr(headerEnd + 4);

    size_t lineEnd = headerPart.find("\r\n");
    if (lineEnd == std::string::npos)
        return ParseResult::INCOMPLETE;

    requestLine = headerPart.substr(0, lineEnd);
    headerLines = headerPart.substr(lineEnd + 2);

	return ParseResult::ALL_OK;
}

HttpParser::ParseResult HttpParser::parseRequest(std::string& buffer, HttpRequest& request)
{
	std::string requestLine;
	std::string headerLines;
	std::string bodyPart;
	ParseResult result;

	result = getRequestParts(buffer, requestLine, headerLines, bodyPart);
	if (result != ParseResult::ALL_OK)
        return result;

	result = parseRequestLine(requestLine, request);
	if (result != ParseResult::ALL_OK)
        return result;

	result = parseHeader(headerLines, request);
	if (result != ParseResult::ALL_OK)
        return result;

	return parseBody(bodyPart, request);
}
