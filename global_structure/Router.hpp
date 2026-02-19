#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "WebServer.hpp"
#include "Action.hpp"

class Router
{
	private:
		const Config& _config;

	public:
		Router(const Config& config);
		Router(const Router& other);
		Router& operator=(const Router& other);
		~Router();

		Action route(const HttpRequest& request, int port);
};

#endif