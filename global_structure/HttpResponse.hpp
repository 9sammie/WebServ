#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "string"
#include "map"

class HttpResponse
{
	public:
		HttpResponse();
		HttpResponse(const HttpResponse& other);
		HttpResponse& operator=(const HttpResponse& other);
		~HttpResponse();
	
    	int statusCode;
    	std::map<std::string, std::string> headers;
    	std::string body;

    	std::string build() const;
};

#endif