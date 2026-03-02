#include "HttpParser.hpp"

HttpParser::ParseResult HttpParser::tockeniseRequestLine(const std::string& requestLine,
																std::string& method,
																std::string& path,
																std::string& version)
{
	size_t firstSpace;
	size_t pathStart;
	size_t secondSpace;
	size_t versionStart;
	size_t versionEnd;

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
	return ParseResult::ALL_OK;
}
