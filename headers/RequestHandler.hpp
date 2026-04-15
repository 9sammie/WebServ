#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "RequestParser.hpp"
#include "Client.hpp"
#include "HttpRequest.hpp"
#include "UriResolver.hpp"
#include "Config.hpp"
#include "HttpException.hpp"
#include "CgiHandler.hpp"

#include <string>
#include <map>
#include <fstream>

class RequestHandler;
typedef std::string (RequestHandler::*MethodHandler)(const HttpRequest&, const std::string&, const LocationConfig*, Client&);

class RequestHandler
{
	public:
		RequestHandler(const ServerConfig& config);
		RequestHandler(const RequestHandler& other);
		~RequestHandler();

		std::string handleRequest(Client& Client);
		int extractStatusCode(const std::string& response) const;
		static std::string buildHttpResponse(int statusCode,
									const std::string& reason,
									const std::string& body,
									const bool closeConnection,
									const std::map<std::string, std::string>& extraHeaders
									= std::map<std::string, std::string>());
		std::string mimeType;
	
	private:
		std::map<std::string, MethodHandler> _methodHandlers;
		const ServerConfig& _config;
		HttpParser			_parser;
		bool _closeConnection;

		std::string executeMethodHandler(Client& Client, const HttpRequest& request, const std::string& fullPath, const LocationConfig* loc);
		std::string handleGET(const HttpRequest& request, const std::string& path, const LocationConfig* loc, Client& Client);
		std::string getMimeType(const std::string& path);
		std::string validateParsing(Client& Client, HttpRequest& request);
		std::string validateLocation(Client& Client, HttpRequest& request, const LocationConfig*& loc, std::string& fullPath);
		std::string	handleCgiExecution(Client& Client, HttpRequest& request, const LocationConfig* loc, std::string& fullPath);
		bool		resolvePath(const HttpRequest& request, const std::string& path, const LocationConfig* loc,
										std::string& outPath, std::string& outResponse, Client& Client);
		DataCgi		fillCgiData(const HttpRequest& req, const std::string& fullPath, const LocationConfig* loc, Client& client);
		bool		isCgiRequest(const std::string& fullPath, const LocationConfig* loc);
		std::string handlePOST(const HttpRequest& request, const std::string& path, const LocationConfig* loc, Client& Client);
		std::string handleDELETE(const HttpRequest& request, const std::string& path, const LocationConfig* loc, Client& Client);
		std::string buildStatusResponse(int code, Client& Client) const;
		std::string getStatusMessage(int code) const;
		void		applyHtmlTemplates(std::string& body, const HttpRequest& request);
		std::string updateCloseStatus(Client& client, const std::string& response);
};

#endif