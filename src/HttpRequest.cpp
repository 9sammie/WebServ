#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
	:	method(""),
		path(""),
		version(""),
		body("")
{}

HttpRequest::HttpRequest(const HttpRequest& other)
{
	method = other.method;
	path = other.path;
	version = other.version;
	headers = other.headers;
	body = other.body;
}

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
	if (this != &other)
	{
		method = other.method;
		path = other.path;
		version = other.version;
		headers = other.headers;
		body = other.body;
	}
	return *this;
}

HttpRequest::~HttpRequest() {}

bool HttpRequest::isComplete() const
{

}