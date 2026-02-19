#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "HttpRequest.hpp"

class HttpParser
{
	private:
		enum State
		{
			REQUEST_LINE,
			HEADERS,
			BODY,
			COMPLETE
		};
		State _state;

	public:
		HttpParser();
		HttpParser(const HttpParser& other);
		HttpParser& operator=(const HttpParser& other);
		~HttpParser();

		bool parse(std::string& buffer, HttpRequest& request);
};

#endif