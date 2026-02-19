#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "string"
#include "map"

class HttpRequest
{
	public:
		HttpRequest();
		HttpRequest(const HttpRequest& other);
		HttpRequest& operator=(const HttpRequest& other);
		~HttpRequest();
	
    	std::string method;
    	std::string path;
    	std::string version;
    	std::map<std::string, std::string> headers;
    	std::string body;
	
    	bool isComplete() const;
};

#endif