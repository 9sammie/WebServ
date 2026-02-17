#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <vector>
#include "ServerConfig.hpp"

// Config structure shema:
//
// Config
//  └── vector<ServerConfig>
//          └── ServerConfig
//                 ├── port = 8080
//                 ├── server_name = example.com
//                 └── vector<LocationConfig>
//                        └── LocationConfig
//                               ├── path = /images
//                               ├── root = ./assets/img
//                               └── allowed_methods = [GET]

class Config
{
	private:
	std::vector<ServerConfig> _servers;

	public:
	Config();
	Config(const Config& other);
	Config& operator=(const Config& other);
	~Config();

	int parseFile(const std::string& path);
	const std::vector<ServerConfig>& getServers() const;
};

#endif