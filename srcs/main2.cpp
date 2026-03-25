#include <iostream>
#include <vector>
#include <exception>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Config.hpp"

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
		std::cout << "Number of servers: " << config.servers.size() << std::endl;

		for (std::size_t i = 0; i < config.servers.size(); ++i)
		{
			const ServerConfig& srv = config.servers[i];
			std::cout << "server[" << i << "]" << std::endl;
			std::cout << "  listens: " << srv.listens.size() << std::endl;

			for (std::size_t j = 0; j < srv.listens.size(); ++j)
			{
				std::cout << "    listen[" << j << "]: "
				          << srv.listens[j].host << ":"
				          << srv.listens[j].port << std::endl;
			}

			std::cout << "  server_name: " << srv.serverName << std::endl;
			std::cout << "  keepalive_timeout: " << srv.keepaliveTimeoutSec << std::endl;
			std::cout << "  client_max_body_size: " << srv.maxBodySize << std::endl;
			std::cout << "  locations: " << srv.locations.size() << std::endl;

			for (std::size_t k = 0; k < srv.locations.size(); ++k)
			{
				const LocationConfig& loc = srv.locations[k];
				std::cout << "    location[" << k << "]: " << loc.prefix << std::endl;
				std::cout << "      root: " << loc.root << std::endl;
				std::cout << "      index: " << loc.index << std::endl;
				std::cout << "      autoindex: " << (loc.autoindex ? "on" : "off") << std::endl;
				std::cout << "      keepalive_timeout: " << loc.keepaliveTimeoutSec << std::endl;
				std::cout << "      client_max_body_size: " << loc.maxBodySize << std::endl;

				std::cout << "      methods:";
				for (std::size_t m = 0; m < loc.methods.size(); ++m)
					std::cout << " " << loc.methods[m];
				std::cout << std::endl;

				if (loc.hasRedirection)
				{
					std::cout << "      return: "
					          << loc.redirectCode << " "
					          << loc.redirectTarget << std::endl;
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 2;
	}

	return 0;
}
