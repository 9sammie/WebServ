#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "HttpRequest.hpp"

class HttpParser
{
	public:
		HttpParser();
		HttpParser(const HttpParser& other);
		HttpParser& operator=(const HttpParser& other);
		~HttpParser();

		bool parse(std::string& buffer, HttpRequest& request);
};

#endif