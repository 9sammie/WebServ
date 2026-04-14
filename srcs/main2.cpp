#include <iostream>
#include <exception>
#include <string>
#include <vector>
#include <map>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Config.hpp"

static void printBool(const std::string& name, bool value, const std::string& indent)
{
	std::cout << indent << name << ": " << (value ? "true" : "false") << std::endl;
}

static void printMethods(const std::vector<std::string>& methods, const std::string& indent)
{
	std::cout << indent << "methods: ";
	if (methods.empty())
	{
		std::cout << "(empty)";
	}
	else
	{
		for (std::size_t i = 0; i < methods.size(); ++i)
		{
			if (i > 0)
				std::cout << ", ";
			std::cout << methods[i];
		}
	}
	std::cout << std::endl;
}

static void printErrors(const std::map<int, std::string>& errors, const std::string& indent)
{
	std::cout << indent << "errors:" << std::endl;
	if (errors.empty())
	{
		std::cout << indent << "  (empty)" << std::endl;
		return;
	}
	for (std::map<int, std::string>::const_iterator it = errors.begin(); it != errors.end(); ++it)
		std::cout << indent << "  " << it->first << " -> " << it->second << std::endl;
}

static void printListen(const ListenConfig& listen, std::size_t i, const std::string& indent)
{
	std::cout << indent << "listen[" << i << "]:" << std::endl;
	std::cout << indent << "  host: " << (listen.host.empty() ? "(empty)" : listen.host) << std::endl;
	std::cout << indent << "  port: " << listen.port << std::endl;
}

static void printLocation(const LocationConfig& loc, std::size_t i, const std::string& indent)
{
	std::cout << indent << "location[" << i << "]:" << std::endl;
	std::cout << indent << "  prefix: " << (loc.prefix.empty() ? "(empty)" : loc.prefix) << std::endl;
	std::cout << indent << "  root: " << (loc.root.empty() ? "(empty)" : loc.root) << std::endl;
	std::cout << indent << "  alias: " << (loc.alias.empty() ? "(empty)" : loc.alias) << std::endl;
	std::cout << indent << "  index: " << (loc.index.empty() ? "(empty)" : loc.index) << std::endl;
	printBool("autoindex", loc.autoindex, indent + "  ");
	printMethods(loc.methods, indent + "  ");
	printBool("uploadAuthorised", loc.uploadAuthorised, indent + "  ");
	printBool("cgiAuthorised", loc.cgiAuthorised, indent + "  ");
	std::cout << indent << "  cgiExt: " << (loc.cgiExt.empty() ? "(empty)" : loc.cgiExt) << std::endl;
	std::cout << indent << "  cgiPath: " << (loc.cgiPath.empty() ? "(empty)" : loc.cgiPath) << std::endl;
	printBool("hasRedirection", loc.hasRedirection, indent + "  ");
	std::cout << indent << "  redirectCode: " << loc.redirectCode << std::endl;
	std::cout << indent << "  redirectTarget: " << (loc.redirectTarget.empty() ? "(empty)" : loc.redirectTarget) << std::endl;
	printBool("hasMaxBodySize", loc.hasMaxBodySize, indent + "  ");
	std::cout << indent << "  maxBodySize: " << loc.maxBodySize << std::endl;
	printBool("hasCgiTimeout", loc.hasCgiTimeout, indent + "  ");
	std::cout << indent << "  cgiTimeoutSec: " << loc.cgiTimeoutSec << std::endl;
}

static void printServer(const ServerConfig& srv, std::size_t i, const std::string& indent)
{
	std::cout << indent << "server[" << i << "]:" << std::endl;
	printBool("hasKeepalive", srv.hasKeepalive, indent + "  ");
	std::cout << indent << "  keepaliveTimeoutSec: " << srv.keepaliveTimeoutSec << std::endl;

	std::cout << indent << "  listens:" << std::endl;
	if (srv.listens.empty())
		std::cout << indent << "    (empty)" << std::endl;
	else
	{
		for (std::size_t j = 0; j < srv.listens.size(); ++j)
			printListen(srv.listens[j], j, indent + "    ");
	}

	std::cout << indent << "  serverName: " << (srv.serverName.empty() ? "(empty)" : srv.serverName) << std::endl;
	printBool("hasMaxBodySize", srv.hasMaxBodySize, indent + "  ");
	std::cout << indent << "  maxBodySize: " << srv.maxBodySize << std::endl;
	printErrors(srv.errors, indent + "  ");
	std::cout << indent << "  root: " << (srv.root.empty() ? "(empty)" : srv.root) << std::endl;
	std::cout << indent << "  index: " << (srv.index.empty() ? "(empty)" : srv.index) << std::endl;

	std::cout << indent << "  locations:" << std::endl;
	if (srv.locations.empty())
		std::cout << indent << "    (empty)" << std::endl;
	else
	{
		for (std::size_t j = 0; j < srv.locations.size(); ++j)
			printLocation(srv.locations[j], j, indent + "    ");
	}
}

static void printHttp(const HttpConfig& http)
{
	std::cout << "HttpConfig:" << std::endl;
	printBool("hasKeepalive", http.hasKeepalive, "  ");
	std::cout << "  keepaliveTimeoutSec: " << http.keepaliveTimeoutSec << std::endl;
	printBool("hasMaxBodySize", http.hasMaxBodySize, "  ");
	std::cout << "  maxBodySize: " << http.maxBodySize << std::endl;
	printErrors(http.errors, "  ");

	std::cout << "  servers:" << std::endl;
	if (http.servers.empty())
		std::cout << "    (empty)" << std::endl;
	else
	{
		for (std::size_t i = 0; i < http.servers.size(); ++i)
			printServer(http.servers[i], i, "    ");
	}
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return 1;
	}

	try
	{
		Lexer lexer;
		std::vector<Token> tokens = lexer.lexFile(argv[1]);

		Parser parser(tokens);
		HttpConfig config = parser.parseConfig();

		std::cout << "Config parsed successfully." << std::endl;
		printHttp(config);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 2;
	}

	return 0;
}