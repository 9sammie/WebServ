#include "RequestHandler.hpp"
#include "Client.hpp"
#include <stdio.h>


DataCgi RequestHandler::fillCgiData(const HttpRequest& req, const std::string& fullPath, const LocationConfig* loc, Client& client)
{
	DataCgi data;

	data.method = req.getMethod();
	data.serverProtocol = req.getVersion();
	data.requestURI = req.getUri();
	data.headers = req.getHeaders();
	data.body = req.getBody();

	size_t qPos = data.requestURI.find('?');
	if (qPos != std::string::npos)
		data.queryString = data.requestURI.substr(qPos + 1);

	data.scriptPath = fullPath;
	data.interpreter = loc->cgiPath;
	data.serverName = _config.serverName;
	data.documentRoot = loc->root.empty() ? _config.root : loc->root;

	data.remoteAddr = client.getCgiInfo().pid;
	data.serverPort = client.getPort(Client::CLIENT);
	data.gateWayInterface = "CGI/1.1";
	data.redirectStatus = "200";

	return data;
}

bool RequestHandler::isCgiRequest(const std::string& fullPath, const LocationConfig* loc)
{
    if (loc->cgiExt.empty()) return false;
    size_t dotPos = fullPath.find_last_of('.');
    if (dotPos == std::string::npos) return false;
    return (fullPath.substr(dotPos) == loc->cgiExt);
}
