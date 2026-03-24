#include "RequestParser.hpp"
#include <sstream>
#include <cerrno>
#include <cstdlib>
#include <climits>

HttpParser::HttpParser() {}

HttpParser::HttpParser(const HttpParser& other) {}

// HttpParser& HttpParser::operator=(const HttpParser& other)
// {
// 	return *this;
// }

HttpParser::~HttpParser() {}




// If there is a body, then retrieve and check out his size conformity.
void HttpParser::parseBody(const std::string& bodyPart, HttpRequest& tempRequest, const ServerConfig& _config)
{
	unsigned long	len;
	std::string		value;
	char*			end;

	if (!tempRequest.hasHeader("content-length"))
	{
		if (!bodyPart.empty())
			throw HttpException(203, "unexpected body");

		tempRequest.setBody("");
		tempRequest.setContentLength(0);
		return;
	}

	value = tempRequest.getHeader("content-length");
	errno = 0;
	len = std::strtoul(value.c_str(), &end, 10);

	if ((errno == ERANGE && len == ULONG_MAX) || (errno != 0 && len == 0))
		throw HttpException(400, "invalid Content-Length");

	if (errno != 0 || *end != '\0')
			throw HttpException(400, "invalid Content-Length");

	if (len > _config.maxBodySize)
		throw HttpException(413, "request entity too large");

	if (bodyPart.size() < len)
		throw HttpException(203, "incomplete body");

	if (bodyPart.size() > len)
		throw HttpException(400, "body size mismatch");
	
	tempRequest.setContentLength(len);
	tempRequest.setBody(bodyPart.substr(0, len));
}

void HttpParser::parseHeaders(const std::string& headersBlock, HttpRequest& tempRequest)
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
			throw HttpException(203, "invalid header format");
		colon = line.find(':');
		if (colon == std::string::npos)
		throw HttpException(203, "invalid header format");

		key = line.substr(0, colon);
		value = line.substr(colon + 1);

		start = value.find_first_not_of(' ');
		if (start == std::string::npos)
			throw HttpException(203, "unexpected error");
		value = value.substr(start);
		if (key.empty())
			throw HttpException(203, "invalid header format");

		tempRequest.setHeader(key, value);
	}
}

bool	checkPath(std::string path)
{
	unsigned char c;

	if (path.empty() || path[0] != '/' || path.find(' ') != std::string::npos
		|| path.find("//") != std::string::npos || path.find("..") != std::string::npos)
		return 1;

	for (size_t i = 0; i < path.size(); ++i)
	{
	    c = path[i];
	    if (std::iscntrl(c))
	        return 1;
	}

	return 0;
}

void HttpParser::tockeniseRequestLine(const std::string& requestLine,
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
		throw HttpException(203, "invalid request line");
	method = requestLine.substr(0, firstSpace);

	pathStart = requestLine.find_first_not_of(' ', firstSpace);
	if (pathStart == std::string::npos)
		throw HttpException(203, "invalid request line");
	secondSpace = requestLine.find(' ', firstSpace + 1);
	if (secondSpace == std::string::npos)
		throw HttpException(203, "invalid request line");
	path = requestLine.substr(pathStart, secondSpace - pathStart);

	versionStart = requestLine.find_first_not_of(' ', secondSpace);
	if (versionStart == std::string::npos)
		throw HttpException(203, "invalid request line");
	versionEnd = requestLine.find(' ', versionStart);
	if (versionEnd == std::string::npos)
	    version = requestLine.substr(versionStart);
	else
	{
		if (requestLine.find_first_not_of(' ', versionEnd) != std::string::npos)
			throw HttpException(203, "invalid request line");
	    version = requestLine.substr(versionStart, versionEnd - versionStart);
	}
}

// Tockenise and only accept "method" "path" "version" before checking
// their content. 
void HttpParser::parseRequestLine(const std::string& requestLine, HttpRequest& tempRequest)
{
	std::string method;
	std::string path;
	std::string version;

	tockeniseRequestLine(requestLine, method, path, version);

	if (method != "GET" && method != "POST" && method != "DELETE")
		throw HttpException(203, "invalid method");

	if (checkPath(path))
		throw HttpException(203, "invalid uri");

	if (version != "HTTP/1.1")
		throw HttpException(203, "invalid http version");

	tempRequest.setMethod(method);
	tempRequest.setUri(path);
	tempRequest.setVersion(version);
}

void HttpParser::getRequestParts(const std::string& buffer, std::string& requestLine, std::string& headerLines, std::string& bodyPart)
{
	size_t headerEnd = buffer.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
		throw HttpException(203, "incomplete");
    std::string headerPart = buffer.substr(0, headerEnd);
    bodyPart = buffer.substr(headerEnd + 4);

    size_t lineEnd = headerPart.find("\r\n");
    if (lineEnd == std::string::npos)
		throw HttpException(203, "incomplete");

    requestLine = headerPart.substr(0, lineEnd);
    headerLines = headerPart.substr(lineEnd + 2);
}



void HttpParser::parseRequest(const std::string& buffer, HttpRequest& request, const ServerConfig& _config)
{
	HttpRequest tempRequest;
	std::string requestLine;
	std::string headerLines;
	std::string bodyPart;

	getRequestParts(buffer, requestLine, headerLines, bodyPart);

	parseRequestLine(requestLine, tempRequest);

	parseHeaders(headerLines, tempRequest);

	parseBody(bodyPart, tempRequest, _config);

	request = tempRequest;
}
