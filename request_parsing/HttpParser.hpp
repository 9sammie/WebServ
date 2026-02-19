#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "HttpRequest.hpp"
#include <string>

class HttpParser
{
	public:
		HttpParser();
		HttpParser(const HttpParser& other);
		HttpParser& operator=(const HttpParser& other);
		~HttpParser();

		// use it like this outside of the class:
		//
		// HttpParser::ParseResult result = parser.parseRequest(...);
		// if (result == HttpParser::ParseResult::SOME_ERROR)
		// response.setStatus(400); Peut etre creer une fonction qui
		// prend en parametre l'erreur, l'associe a un http status code,
		// et renvoi ce status code.
		enum class ParseResult
		{
			ALL_OK,
			INCOMPLETE,

			INVALID_REQUEST_LINE,
    		INVALID_METHOD,
    		INVALID_URI,
    		INVALID_HTTP_VERSION,

    		INVALID_HEADER_FORMAT,
    		HEADER_TOO_LARGE,
    		DUPLICATE_CONTENT_LENGTH,
    		INVALID_CONTENT_LENGTH,
    		MISSING_HOST_HEADER,

    		BODY_TOO_LARGE,
    		BODY_SIZE_MISMATCH,
    		INVALID_TRANSFER_ENCODING,
    		CHUNKED_FORMAT_ERROR,

			UNEXPECTED_ERROR
		};

		ParseResult getRequestParts(std::string& buffer, std::string& requestLine, std::string& headerLines, std::string& bodyPart);
		ParseResult parseRequest(std::string& buffer, HttpRequest& request);
		ParseResult parseRequestLine(const std::string& requestLine, HttpRequest& request);
		ParseResult parseHeader(const std::string& headerLines, HttpRequest& request);
		ParseResult parseBody(const std::string& bodyPart, HttpRequest& request);
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