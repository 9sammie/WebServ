*This project has been created as part of the 42 curriculum by ctheveno, maballet, vakozhev.*

# webserv

## Description

`webserv` is a custom HTTP server written in C++98. The purpose of this project is to understand how a web server works internally.  

For our webserver, we took inspiration from NGINX, but we developed our own configuration file and our own implementation approach.

The project covers some server functionalities such as accepting client connections, reading HTTP requests, building and sending HTTP responses, serving static files, handling routes and managing errors.

---

### Main Features

- Configuration file parsing, handling `http`, `server`, and `location` blocks and multiple ports
- Handling sockets
- Non-blocking or event-driven connection handling based on `poll()` mechanism
- Static website serving
- Route-based configuration
- Accurate HTTP status code handling
- Implementation of allowed methods: `DELETE, POST, GET`
- Providing custom error pages
- Handling redirections
- CGI support
- Upload handling

---

### Project Structure

This project is organized around a few important components:

- `Configuration file` to define the server settings and behavior

- `Parsing part` to read the configuration file, to validate its syntax and to convert it into the in-memory structure used by the server

- `HTTP server setup, client connection handling` to initialize the listening sockets, to prepare the server to accept connections on the configured ports; to manage client sockets, to receive incoming data  

- `HTTP request parsing, HTTP response generation, route handling and content selection` to ensure that requests are processed without blocking indefinitely, to interpret raw data as HTTP requests by extracting the method, headers and body; to create valid HTTP response

---

### Config file parsing approach

The parsing of the configuration file has been divided into two separate parts:

- the lexer is responsible for tokenization,
- the parser is responsible for syntax validation and configuration building.
The configuration structure is responsible for storing parsed data.

#### 1. Lexical analysis

The lexer reads the configuration file and transforms it into a list of tokens. It detects:
- words,
- braces (`{` and `}`),
- semicolons,
- comments,
- and include directives if supported

#### 2. Syntax parsing

The parser consumes the token sequence and builds the final configuration objects in memory.

It validates:
- where a directive is allowed,
- how many arguments it accepts,
- whether values are valid,
- whether methods are supported,
- whether duplicate values are present,
- whether blocks are properly opened and closed.

For example:
- `http` is only valid at the top level,
- `server` is only valid inside `http`,
- `location` is only valid inside `server`.

### TO ADD SERVER SETUP AND PARSING OF REQUEST / BUILD RESPONSE

---

## Instructions

```bash
# Clone the repository
git clone git@github.com:9sammie/WebServ.git

# Enter the project directory
cd WebServ

# Build the project
make

# Build and execute the tests for Lexer and Parser parts
make tests

# Run the server with a custom configuration file
./webserv <path_to_config_file>

```
---
## Resources

### General documentation
- [C++ Web Programming](https://www.tutorialspoint.com/cplusplus/cpp_web_programming.htm)

### HTTP
- [Hypertext Transfer Protocol -- HTTP/1.0](https://datatracker.ietf.org/doc/html/rfc1945)
- [The Common Gateway Interface (CGI) Version 1.1](https://datatracker.ietf.org/doc/html/rfc3875)
- [HTTP reference](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference)
- [Overview of HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP/Overview)
- [HTTP messages](https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages)
- [HTTP request methods](https://en.wikipedia.org/wiki/HTTP#Request_methods)
- [HTTP error codes](https://httpstatusdogs.com/?utm_source=chatgpt.com)
- [HTTP response status codes](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status?utm_source=chatgpt.com)

### CGI
- [Getting Started with CGI Programs](http://www.mnuwer.dbasedeveloper.co.uk/dlearn/web/session01.htm)

### Cookies
- [Using HTTP cookies](https://developer.mozilla.org/en-US/docs/Web/HTTP/Cookies)
- [HTTP Cookies Explained With a Simple Diagram](https://bytebytego.com/guides/http-cookies-explained-with-a-simple-diagram/)

### poll()
- [Using poll() instead of select()](https://www.ibm.com/docs/en/i/7.4.0?topic=designs-using-poll-instead-select)

### NGINX
- [nginx](https://nginx.org/)
- [nginx configuration guide](https://nginx.org/en/docs/)

### Use of AI tools
- to clarify some HTTP concepts
- to review parsing and configuration design
- to generate HTML template examples
- to help with Markdown formatting while writing the README
