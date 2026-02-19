#include "HttpResponse.hpp"

HttpResponse::HttpResponse()
	:	statusCode(0),
		body("")
{}

HttpResponse::HttpResponse(const HttpResponse& other)
{
	statusCode = other.statusCode;
	headers = other.headers;
	body = other.body;
}

HttpResponse& HttpResponse::operator=(const HttpResponse& other)
{
	if (this != &other)
	{
		statusCode = other.statusCode;
		headers = other.headers;
		body = other.body;
	}
	return *this;
}

HttpResponse::~HttpResponse() {}

std::string HttpResponse::build() const
{

}