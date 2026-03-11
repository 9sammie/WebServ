#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "RequestParser.hpp"
#include "Client.hpp"
#include "HttpRequest.hpp"
#include "UriResolver.hpp"
#include "Config.hpp"
#include "HttpException.hpp"

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
	
	private:
		const ServerConfig& _config;
		HttpParser			_parser;

	std::string handleGET(const std::string& path);
	std::string handlePOST(const HttpRequest& request, const std::string& path);
	std::string handleDELETE(const std::string& path);

	std::string buildHttpResponse(int statusCode, const std::string& reason,
                const std::string& body,
                const std::map<std::string, std::string>& extraHeaders = {}) const;
};

#endif