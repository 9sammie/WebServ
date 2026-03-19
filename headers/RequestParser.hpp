#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "HttpRequest.hpp"

class HttpParser
{
	public:
		HttpParser();
		HttpParser(const HttpParser& other);
		// HttpParser& operator=(const HttpParser& other);
		~HttpParser();

		void parseRequest(const std::string& buffer, HttpRequest& request);
		void getRequestParts(const std::string& buffer, std::string& requestLine, std::string& headerLines, std::string& bodyPart);
		
		void tockeniseRequestLine(const std::string& requestLine, std::string& method, std::string& path, std::string& version);
		void parseRequestLine(const std::string& requestLine, HttpRequest& tempRequest);
		void parseHeaders(const std::string& headerBlock, HttpRequest& tempRequest);
		void parseBody(const std::string& bodyPart, HttpRequest& tempRequest);
};

#endif

// Explication des principales erreur:
// 
// 
// INVALID_REQUEST_LINE
// Ligne ne contient pas 3 parties (METHOD PATH VERSION)
// 
// INVALID_METHOD
// Méthode inconnue ou caractères invalides
// 
// INVALID_HTTP_VERSION
// Pas HTTP/1.1 ou mauvais format
// 
// INVALID_HEADER_FORMAT
// Pas de : dans le header
// ou header vide mal formé
// 
// DUPLICATE_CONTENT_LENGTH
// Deux Content-Length différents → attaque possible
// 
// INVALID_CONTENT_LENGTH
// Non numérique
// ou négatif
// ou dépassement de size_t
// 
// MISSING_HOST_HEADER
// En HTTP/1.1 → obligatoire
// 
// BODY_SIZE_MISMATCH
// Content-Length = 10 mais body = 8 ou 15
// 
// INVALID_TRANSFER_ENCODING
// Si Transfer-Encoding existe mais invalide
// 
// CHUNKED_FORMAT_ERROR
// Si tu implémentes chunked plus tard