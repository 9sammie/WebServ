#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Action.hpp"
#include "string"
#include "map"

class ResponseBuilder
{
	public:
		ResponseBuilder();
		ResponseBuilder(const ResponseBuilder& other);
		ResponseBuilder& operator=(const ResponseBuilder& other);
		~ResponseBuilder();
	
	    HttpResponse build(const Action& action, const HttpRequest& request);
};

#endif