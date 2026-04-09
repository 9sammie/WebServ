#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
	:	_method(""),
		_uri(""),
		_version(""),
		_body(""),
		_contentLength(0)
{}

HttpRequest::HttpRequest(const HttpRequest& other)
{
	_method = other._method;
	_uri = other._uri;
	_version = other._version;
	_headers = other._headers;
	_body = other._body;
	_contentLength = other._contentLength;
}

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
	if (this != &other)
	{
		_method = other._method;
		_uri = other._uri;
		_version = other._version;
		_headers = other._headers;
		_body = other._body;
		_contentLength = other._contentLength;
	}
	return *this;
}

HttpRequest::~HttpRequest() {}



/*############################### GETTERS #############################*/


const std::string& HttpRequest::getMethod() const
{
	return _method;
}

const std::string& HttpRequest::getUri() const
{
    return _uri;
}

const std::string& HttpRequest::getVersion() const
{
    return _version;
}

// return one specific header amoung all of them, using find() wich
// returns an iterator/const iterator. Here it acts like a pointer
// to the key of the map passed in parameter.
std::string HttpRequest::getHeader(const std::string& key) const
{
    std::map<std::string, std::string>::const_iterator it = _headers.find(key);
    if (it != _headers.end())
        return it->second;
    return "";
}

const std::map<std::string, std::string>& HttpRequest::getHeaders() const
{
    return _headers;
}

const std::string& HttpRequest::getBody() const
{
    return _body;
}

size_t HttpRequest::getContentLength() const
{
	return _contentLength;
}
#include <fstream>
std::string HttpRequest::getCookie(const std::string& name) const
{
    std::string header = this->getHeader("cookie");
    if (header.empty())
		return "";

    std::string toFind = name + "=";
    size_t start = header.find(toFind);
    if (start == std::string::npos)
		return "";

    start += toFind.length();
    size_t end = header.find(';', start);
    return header.substr(start, end - start);
}

std::string HttpRequest::getQueryParam(const std::string& key) const 
{
	std::string uri = this->getUri();
	size_t queryStart = uri.find('?');

	if (queryStart == std::string::npos)
		return "";

	std::string queryString = uri.substr(queryStart + 1);
	std::string toFind = key + "=";
	size_t pos = queryString.find(toFind);

	while (pos != std::string::npos)
	{
		if (pos == 0 || queryString[pos - 1] == '&')
		{
			size_t valueStart = pos + toFind.length();
			size_t valueEnd = queryString.find('&', valueStart);

			return queryString.substr(valueStart, valueEnd - valueStart);
		}
		pos = queryString.find(toFind, pos + 1);
	}
	return "";
}



/*############################### SETTERS #############################*/


void HttpRequest::setMethod(const std::string& method)
{
    _method = method;
}

void HttpRequest::setUri(const std::string& uri)
{
    _uri = uri;
}

void HttpRequest::setVersion(const std::string& version)
{
    _version = version;
}

void HttpRequest::setHeader(const std::string& key,
                            const std::string& value)
{
    _headers[key] = value;
}

void HttpRequest::setBody(const std::string& body)
{
    _body = body;
}

void HttpRequest::setContentLength(size_t contentLength)
{
	_contentLength = contentLength;
}



/*############################### OTHERS #############################*/


bool HttpRequest::hasHeader(const std::string& key) const
{
    return _headers.find(key) != _headers.end();
}