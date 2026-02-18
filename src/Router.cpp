#include "Router.hpp"

Router::Router(const Config& config): _config(config) {}

Router::Router(const Router& other): _config(other._config) {}

Router& Router::operator=(const Router& other)
{
	(void)other;
	return *this;
}

Router::~Router() {}
