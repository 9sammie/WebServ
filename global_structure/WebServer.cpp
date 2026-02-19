#include "WebServer.hpp"

WebServer::WebServer() {}

WebServer::WebServer(const WebServer& other)
{
	_config = other._config;
}

WebServer& WebServer::operator=(const WebServer& other)
{
	_config = other._config;
	return *this;
}

WebServer::~WebServer() {}

void run()
{

}