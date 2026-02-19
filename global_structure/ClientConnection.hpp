#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include "Config.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <vector>

// ClientConnection structure schema:
//
// ClientConnection
//  ├── fd
//  │     └── 7 (socket du client)
//  │
//  ├── readBuffer
//  │     └── "GET /index.html HTTP/1.1\r\nHost: example.com\r\n..."
//  │
//  ├── writeBuffer
//  │     └── "HTTP/1.1 200 OK\r\nContent-Length: 123\r\n..."
//  │
//  ├── HttpRequest request
//  │     ├── method = "GET"
//  │     ├── path = "/index.html"
//  │     ├── version = "HTTP/1.1"
//  │     ├── headers
//  │     │     ├── "Host" -> "example.com"
//  │     │     ├── "Content-Length" -> "42"
//  │     │     └── ...
//  │     └── body
//  │           └── "" (vide si GET)
//  │
//  ├── HttpResponse response
//  │     ├── status = 200
//  │     ├── headers
//  │     │     ├── "Content-Type" -> "text/html"
//  │     │     ├── "Content-Length" -> "123"
//  │     │     └── ...
//  │     └── body
//  │           └── "<html>...</html>"
//  │
//  └── state

class ClientConnection
{
	public:
		ClientConnection();
		ClientConnection(const ClientConnection& other);
		ClientConnection& operator=(const ClientConnection& other);
		~ClientConnection();

    	int fd;
    	std::string readBuffer;
    	std::string writeBuffer;
    	HttpRequest request;
    	HttpResponse response;

    	enum State {
    	    READING_HEADERS,
    	    READING_BODY,
    	    PROCESSING,
    	    WRITING,
    	    DONE
    	};
    	State state;
};

#endif