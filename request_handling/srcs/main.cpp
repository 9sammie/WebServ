#include "RequestParser.hpp"
#include "iostream"
#include "fstream"

void printMyPart(const HttpRequest& request)
{
    std::cout << "===== REQUEST =====" << std::endl;

    std::cout << "Method: " << request.getMethod() << std::endl;
    std::cout << "Path: " << request.getPath() << std::endl;
    std::cout << "Version: " << request.getVersion() << std::endl;

    std::cout << "Headers: \n" << std::endl;
    const std::map<std::string, std::string>& headers = request.getHeaders();

    for (std::map<std::string, std::string>::const_iterator it = headers.begin();
         it != headers.end();
         ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }

    std::cout << "Content-Length: " << request.getContentLength() << std::endl;

    if (!request.getBody().empty())
    {
        std::cout << "Body: " << std::endl;
        std::cout << request.getBody() << std::endl;
    }
}

int main(int argc, char **argv)
{
	HttpParser parser;
	HttpRequest request;

	if (argc != 2)
		return 1;

	std::string httpRequest = 
    "POST /users HTTP/1.1\r\n"
    "Host: example.com\r\n"
    "Content-Length: 49\r\n"
    "\r\n"
    "name=FirstName+LastName&email=bsmth%40example.com";
	parser.parseRequest(httpRequest, request);

	printMyPart(request);
	return 0;
}
