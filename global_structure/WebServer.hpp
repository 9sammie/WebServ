#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "ClientConnection.hpp"
#include "Config.hpp"
#include <vector>

// WebServer structure schema:
//
// WebServer
//  ├── vector<int> _listenSockets
//  │       ├── 3   (port 8080)
//  │       ├── 4   (port 9090)
//  │       └── ...
//  │
//  ├── vector<ClientConnection*> _clients
//  │       └── ClientConnection
//  │              ├── fd = 7
//  │              ├── readBuffer
//  │              ├── writeBuffer
//  │              ├── HttpRequest request
//  │              │
//  │              ├── HttpResponse response
//  │              │
//  │              └── state
//  │                     ├── READING_HEADERS
//  │                     ├── READING_BODY
//  │                     ├── PROCESSING
//  │                     ├── WRITING
//  │                     └── DONE
//  │
//  └── Config _config
//          └── vector<ServerConfig>
//                 └── ServerConfig
//                        ├── port
//                        ├── serverName
//                        ├── root
//                        └── vector<LocationConfig>

class WebServer
{
	private:
	    std::vector<int> _listenSockets;
	    std::vector<ClientConnection*> _clients;
	    Config _config;

	public:
		WebServer();
		WebServer(const WebServer& other);
		WebServer& operator=(const WebServer& other);
		~WebServer();

	    void run();
};

#endif