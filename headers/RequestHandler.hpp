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

class RequestHandler
{
	public:
		RequestHandler(const ServerConfig& config);
		RequestHandler(const RequestHandler& other);
		~RequestHandler();

		// Traite le buffer (requête complète ou non) et renvoie :
    	// - une réponse HTTP complète si la requête est complète ou en cas d'erreur,
    	// - une string vide si la requête est incomplète (caller doit attendre + données).
		std::string handleRequest(Client& Client);
		int extractStatusCode(const std::string& response) const;
		static std::string buildHttpResponse(int statusCode,
									const std::string& reason,
									const std::string& body,
									const bool closeConnection,
									const std::map<std::string, std::string>& extraHeaders
									= std::map<std::string, std::string>());
	
	private:
		const ServerConfig& _config;
		HttpParser			_parser;
		bool _closeConnection;

	std::string handleGET(const HttpRequest& request, const std::string& path, const LocationConfig* loc);
	bool		resolvePath(const HttpRequest& request, const std::string& path, const LocationConfig* loc,
									std::string& outPath, std::string& outResponse);
	DataCgi		fillCgiData(const HttpRequest& req, const std::string& fullPath, const LocationConfig* loc, Client& client);
	bool		isCgiRequest(const std::string& fullPath, const LocationConfig* loc);
	std::string handlePOST(const HttpRequest& request, const std::string& path, const LocationConfig* loc);
	std::string handleDELETE(const std::string& path, const LocationConfig* loc);
	std::string buildStatusResponse(int code) const;
};

#endif