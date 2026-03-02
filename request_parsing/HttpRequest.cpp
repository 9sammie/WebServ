#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
	:	_method(""),
		_path(""),
		_version(""),
		_body(""),
		_contentLength(0)
{}

HttpRequest::HttpRequest(const HttpRequest& other)
{
	_method = other._method;
	_path = other._path;
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
		_path = other._path;
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

const std::string& HttpRequest::getPath() const
{
    return _path;
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



/*############################### SETTERS #############################*/


void HttpRequest::setMethod(const std::string& method)
{
    _method = method;
}

void HttpRequest::setPath(const std::string& path)
{
    _path = path;
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