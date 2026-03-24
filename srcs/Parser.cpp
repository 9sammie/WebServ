/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 09:24:15 by vakozhev          #+#    #+#             */
/*   Updated: 2026/03/24 19:03:47 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include <stdexcept>
#include <cstdlib> //strtol, strtoull
#include <cerrno>
#include <limits>
#include <sstream>

Parser::Parser(const std::vector<Token>& toks) : _toks(toks), _pos(0)
{}

const Token& Parser::currentToken() const
{
	if (_toks.empty())
		throw std::invalid_argument("webserv: [emerg] internal parser error");
	if (_pos >= _toks.size())
		return _toks[_toks.size() - 1];
	return _toks[_pos];
}

const Token& Parser::previousToken() const
{
	if (_toks.empty())
		throw std::invalid_argument("webserv: [emerg] internal parser error");
	if (_pos == 0)
		return _toks[0];
	return _toks[_pos -1];
}

std::string Parser::formatError(const Token& tok, const std::string& msg) const
{
	std::ostringstream out;
	out << "webserv: [emerg] " << msg;
	if (tok.line > 0)
		out << " at line " << tok.line;
	return out.str();
}

void Parser::throwUnexpectedEof(const std::string& expected) const
{
	const Token& tok = previousToken();
	throw std::invalid_argument(formatError(tok, "unexpected end of file, expecting \"" + expected + "\""));
}

void Parser::throwUnexpectedToken(const Token& tok) const
{
	if (tok.type == RBRACE)
		throw std::invalid_argument(formatError(tok, "unexpected \"}\""));
	if (tok.type == LBRACE)
		throw std::invalid_argument(formatError(tok, "unexpected \"{\""));
	if (tok.type == SEMICOLON)
		throw std::invalid_argument(formatError(tok, "unexpected \";\""));
	if (tok.type == WORD)
		throw std::invalid_argument(formatError(tok, "unexpected \"" + tok.wordText + "\""));
	if (tok.type == COMMENT)
		throw std::invalid_argument(formatError(tok, "unexpected comment"));
	throw std::invalid_argument(formatError(tok, "unexpected token"));
}

void Parser::throwInvalidArgs(const Token& directiveTok) const
{
	throw std::invalid_argument(formatError(directiveTok, "invalid number of arguments in \"" + directiveTok.wordText + "\" directive"));
}

void Parser::throwUnknownDirective(const Token& directiveTok) const
{
	throw std::invalid_argument(formatError(directiveTok, "unknown directive \"" + directiveTok.wordText + "\""));
}

void Parser::throwDirectiveNotAllowedHere(const Token& directiveTok) const
{
	throw std::invalid_argument(formatError(directiveTok, "\"" + directiveTok.wordText + "\" directive is not allowed here"));
}

void Parser::throwDirectiveNotTerminated(const Token& directiveTok) const
{
	throw std::invalid_argument(formatError(directiveTok, "directive \"" + directiveTok.wordText + "\" is not terminated by \";\""));
}

void Parser::throwNoOpeningBrace(const Token& directiveTok) const
{
	throw std::invalid_argument(formatError(directiveTok, "directive \"" + directiveTok.wordText + "\" has no opening \"{\""));
}

void Parser::throwInvalidValue(const Token& directiveTok, const std::string& value) const
{
	throw std::invalid_argument(formatError(directiveTok, "invalid value \"" + value + "\" in \"" + directiveTok.wordText + "\" directive"));
}

void Parser::throwDuplicateValue(const Token& directiveTok, const std::string& value) const
{
	throw std::invalid_argument(formatError(directiveTok, "duplicate value \"" + value + "\" in \"" + directiveTok.wordText + "\" directive"));
}
	
bool Parser::isNotEnd() const
{
	if (_pos >= _toks.size())
		return false;
	return true;
}

bool Parser::checkType(TokenType expectedType) const
{
	if (!isNotEnd())
		return false;
	if (_toks[_pos].type == expectedType)
		return true;
	return false;
}
		
const Token& Parser::consume(TokenType expectedType)
{
	if (!isNotEnd())
		throwUnexpectedEof("token");
	if (_toks[_pos].type != expectedType)
		throwUnexpectedToken(_toks[_pos]);
	_pos++;
	return _toks[_pos - 1]; //on retourne par ref&
}

bool Parser::checkWord(const std::string& expected) const //il s agit d un MOT et c est un mot que j attends
{
	if (checkType(WORD) && _toks[_pos].wordText == expected)
		return true;
    return false;
}

const Token& Parser::consumeWord(const std::string& expected)
{
	if (!isNotEnd())
		throwUnexpectedEof(expected);
	if (!checkWord(expected))
		throwUnexpectedToken(currentToken());
	return consume(WORD);
}

/*void Parser::skipBlock()
{
	consume(LBRACE);
	int level = 1;
	while (isNotEnd() && level > 0)
	{
		if (checkType(LBRACE))
		{
			consume(LBRACE);
			level++;
		}
		else if (checkType(RBRACE))
		{
			consume(RBRACE);
			level--;
		}
		else
			_pos++;
	}
	if (level != 0)
		throw std::invalid_argument("missing '}' to close block");
}*/

std::vector<std::string> Parser::readDirectiveArgs(const Token& directiveTok)
{
	std::vector<std::string> args;
	while (checkType(WORD))
	{
		const Token& t = consume(WORD);
		args.push_back(t.wordText);
	}
	if (!isNotEnd())
		throwDirectiveNotTerminated(directiveTok);
	if (!checkType(SEMICOLON))
	{
		if (checkType(LBRACE))
			throwDirectiveNotTerminated(directiveTok);
		throwUnexpectedToken(currentToken());
	}
	consume(SEMICOLON);
	return args;
}

bool Parser::parseOnOffArg(const Token& directiveTok, const std::vector<std::string>& args)
{
	if (args.size() != 1)
		throwInvalidArgs(directiveTok);
	if (args[0] == "on")
		return true;
	if (args[0] == "off")
		return false;
	throwInvalidValue(directiveTok, args[0]);
	return false;
}

int Parser::parsePositiveInt(const Token& directiveTok, const std::string& s) const
{
	if (s.empty())
		throwInvalidValue(directiveTok, s);
	char* end = NULL;
	errno = 0;
	long res = std::strtol(s.c_str(), &end, 10);//&end = l adresse de end pour ecrire dedans la valeur
	if (errno != 0 || end == s.c_str() || *end != '\0') //owerflow/underflow, ou aucun chiffre, ou il reste des car 12a p ex
		throwInvalidValue(directiveTok, s);
	if (res < 0)
		throwInvalidValue(directiveTok, s); //positifs
	if (res > std::numeric_limits<int>::max())
		throwInvalidValue(directiveTok, s); //doit rentrer dans int
	return static_cast<int>(res);
}	

std::size_t Parser::parseSizeT(const Token& directiveTok, const std::string& s) const
{
	if (s.empty())
		throwInvalidValue(directiveTok, s);
	if (s[0] == '-')
		throwInvalidValue(directiveTok, s);
	char* end = NULL;
	errno = 0;
	unsigned long long res = std::strtoull(s.c_str(), &end, 10);
	if (errno != 0 || end == s.c_str() || *end != '\0')
		throwInvalidValue(directiveTok, s);
	if (res > static_cast<unsigned long long>(std::numeric_limits<size_t>::max()))
		throwInvalidValue(directiveTok, s);
	return static_cast<size_t>(res);
} 

int Parser::parsePort(const Token& directiveTok, const std::string& s)
{
	int port = parsePositiveInt(directiveTok, s);
	if (port < 1 || port > 65535)
		throwInvalidValue(directiveTok, s);
	return port;
}

static bool isHttpMethod(const std::string& m)
{
	return (m == "GET" || m == "POST" || m == "DELETE");
}

static bool containsMethod(const std::vector<std::string>& v, const std::string& s)
{
	for (std::size_t i = 0; i < v.size(); ++i)
	{
		if (v[i] == s)
			return true;
	}
	return false;
}

void Parser::applyMethods(LocationConfig& loc, const Token& directiveTok, const std::vector<std::string>& args)
{
	if (args.empty())
		throwInvalidArgs(directiveTok);
	loc.methods.clear();
	for (std::size_t i = 0; i < args.size(); ++i)
	{
		std::string m = args[i];
		if (!isHttpMethod(m))
			throwInvalidValue(directiveTok, m);
		if (containsMethod(loc.methods, m))
			throwDuplicateValue(directiveTok, m);
		loc.methods.push_back(m);
	}
	loc.uploadAuthorised = containsMethod(loc.methods, "POST");
}

/////
void Parser::parseErrorPage(std::map<int, std::string>& errors, const Token& directiveTok, const std::vector<std::string>& args)
{
	if (args.size() < 2)
		throwInvalidArgs(directiveTok);
	const std::string& path = args[args.size() - 1];
	for (std::size_t i = 0; i + 1 < args.size(); ++i)
	{
		int code = parsePositiveInt(directiveTok, args[i]);
		if (code < 300 || code > 599)
			throwInvalidValue(directiveTok, args[i]);
		errors[code] = path;
	}
}

void Parser::parseLocationDirective(LocationConfig& loc, const Token& nameTok) //int line garder ?
{
	const std::string& name = nameTok.wordText;
	if (name == "root")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		if (args.size() != 1)
			throwInvalidArgs(nameTok);
		loc.root = args[0];
		return;
	}
	if (name == "index")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		if (args.size() != 1)
			throwInvalidArgs(nameTok);
		loc.index = args[0];
		return;
	}
	if (name == "autoindex")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		loc.autoindex = parseOnOffArg(nameTok, args);
		return;
	}
	if (name == "methods")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		applyMethods(loc, nameTok, args);
		return;
	}
	if (name == "return")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		if (args.size() != 2)
			throwInvalidArgs(nameTok);
		int code = parsePositiveInt(nameTok, args[0]);
		if (code != 301 && code != 302 && code != 307 && code != 308)
			throwInvalidValue(nameTok, args[0]);
		loc.hasRedirection = true;
		loc.redirectCode = code;
		loc.redirectTarget = args[1];
		return;
	}
	if (name == "client_max_body_size")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		if (args.size() != 1)
			throwInvalidArgs(nameTok);
		loc.maxBodySize = parseSizeT(nameTok, args[0]);
		loc.hasMaxBodySize = true;
		return;
	}
	if (name == "cgi_ext")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		if (args.size() != 1)
			throwInvalidArgs(nameTok);
		loc.cgiExt = args[0];
		loc.cgiAuthorised = true;
		return;
	}
	if (name == "cgi_path")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		if (args.size() != 1)
			throwInvalidArgs(nameTok);
		loc.cgiPath = args[0];
		loc.cgiAuthorised = true;
		return;
	}
	throwUnknownDirective(nameTok);
}
	
LocationConfig Parser::parseLocationBlock(const Token& locationTok)
{
    LocationConfig loc;
	if (!checkType(WORD))
	{
		if (!isNotEnd())
			throwUnexpectedEof("location modifier or prefix");
		throwUnexpectedToken(currentToken());
	}
	const Token& prefixTok = consume(WORD); 
    loc.prefix = prefixTok.wordText;

    if (!isNotEnd())
		throwNoOpeningBrace(locationTok);
	if (!checkType(LBRACE))
		throwNoOpeningBrace(locationTok);
	consume(LBRACE);
	while (!checkType(RBRACE))
	{
		if (!isNotEnd())
			throwUnexpectedEof("}");
		if (!checkType(WORD))
			throwUnexpectedToken(currentToken());
        const Token& nameTok = consume(WORD);
        if (nameTok.wordText == "location")
            throwDirectiveNotAllowedHere(nameTok);
        if (nameTok.wordText == "server")
            throwDirectiveNotAllowedHere(nameTok);
		if (nameTok.wordText == "http")
			throwDirectiveNotAllowedHere(nameTok);

        parseLocationDirective(loc, nameTok);
    }
    consume(RBRACE);
    return loc;
}

void Parser::parseHttpDirective(HttpConfig& http, const Token& nameTok)
{
	const std::string& name = nameTok.wordText;
	if (name == "keepalive_timeout")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		if (args.size() != 1)
			throwInvalidArgs(nameTok);
		http.keepaliveTimeoutSec = parsePositiveInt(nameTok, args[0]);
		http.hasKeepalive = true;
		return;
	}
	if (name == "client_max_body_size")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		if (args.size() != 1)
			throwInvalidArgs(nameTok);
		http.maxBodySize = parseSizeT(nameTok, args[0]);
		http.hasMaxBodySize = true;
		return;
	}
	if (name == "error_page")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		parseErrorPage(http.errors, nameTok, args);
		return;
	}
	throwUnknownDirective(nameTok);
}

HttpConfig Parser::parseHttpBlock(const Token& httpTok)
{
	HttpConfig http;
	if (!isNotEnd())
		throwNoOpeningBrace(httpTok);
	if (!checkType(LBRACE))
		throwNoOpeningBrace(httpTok);
	consume(LBRACE);
	while (!checkType(RBRACE))
	{
		if (!isNotEnd())
			throwUnexpectedEof("}");
		if (!checkType(WORD))
			throwUnexpectedToken(currentToken());
		const Token& nameTok = consume(WORD);
		if (nameTok.wordText == "server")
		{
			ServerConfig srv = parseServerBlock(nameTok);
			http.servers.push_back(srv);
			continue;
		}
		if (nameTok.wordText == "location")
			throwDirectiveNotAllowedHere(nameTok);
		parseHttpDirective(http, nameTok);
	}
	consume(RBRACE);
	return http;
}

ListenConfig Parser::parseListenArg(const Token& directiveTok, const std::string& s)
{
	ListenConfig lc;
	lc.host = "";
	lc.port = 0;
	std::size_t pos = s.find(':');//on peut ecrire listen 127.0.0.1:8080; ou listen 8080;
	if (pos == std::string::npos)
	{
		lc.port = parsePort(directiveTok, s);
		return lc;
	}
	std::string hostPart = s.substr(0, pos);
	std::string portPart = s.substr(pos + 1);
	if (hostPart.empty() || portPart.empty()) 
		throwInvalidValue(directiveTok, s);
	lc.host = hostPart;
	lc.port = parsePort(directiveTok, portPart);
	return lc;
}

void Parser::parseServerDirective(ServerConfig& srv, const Token& nameTok)
{
	const std::string& name = nameTok.wordText;
	if (name == "listen")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		if (args.size() != 1)
			throwInvalidArgs(nameTok);
		ListenConfig lc = parseListenArg(nameTok, args[0]);
		srv.listens.push_back(lc);
		return;
	}
	if (name == "server_name")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		if (args.size() != 1)
			throwInvalidArgs(nameTok);
		srv.serverName = args[0];
		return;
	}
	if (name == "keepalive_timeout")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		if (args.size() != 1)
			throwInvalidArgs(nameTok);
		srv.keepaliveTimeoutSec = parsePositiveInt(nameTok, args[0]);
		srv.hasKeepalive = true;
		return;
	}
	if (name == "client_max_body_size")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		if (args.size() != 1)
			throwInvalidArgs(nameTok);
		srv.maxBodySize = parseSizeT(nameTok, args[0]);
		srv.hasMaxBodySize = true;
		return;
	}
	if (name == "error_page")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		parseErrorPage(srv.errors, nameTok, args);//d ou vient ceci ???
		return;
	}
	throwUnknownDirective(nameTok);
}
		
ServerConfig Parser::parseServerBlock(const Token& serverTok)
{
	ServerConfig srv;
	if (!isNotEnd())
		throwNoOpeningBrace(serverTok);
	if(!checkType(LBRACE))
		throwNoOpeningBrace(serverTok);
	consume(LBRACE);
	while (!checkType(RBRACE))
	{
		if (!isNotEnd())
			throwUnexpectedEof("}");
		if (!checkType(WORD))
			throwUnexpectedToken(currentToken());
		const Token& nameTok = consume(WORD);
		if (nameTok.wordText == "server")
			throwDirectiveNotAllowedHere(nameTok);
		if (nameTok.wordText == "http")
			throwDirectiveNotAllowedHere(nameTok);
		if (nameTok.wordText == "location")
		{
			LocationConfig loc = parseLocationBlock(nameTok);
			srv.locations.push_back(loc);
			continue;
		}
		parseServerDirective(srv, nameTok);
	}
	consume(RBRACE);
	if (srv.listens.empty())
		throw std::invalid_argument(formatError(serverTok, "no \"listen\" is defined for server"));
	return srv;
}

/////////////////////////////////func will be modified///////////////
void Parser::applyEffectifData(HttpConfig& http)
{
    for (size_t si = 0; si < http.servers.size(); ++si)
    {
        ServerConfig& srv = http.servers[si];

        // server hérite de http si non défini
        if (!srv.hasKeepalive && http.hasKeepalive)
        {
            srv.keepaliveTimeoutSec = http.keepaliveTimeoutSec;
            srv.hasKeepalive = true;
        }
        if (!srv.hasMaxBodySize && http.hasMaxBodySize)
        {
            srv.maxBodySize = http.maxBodySize;
            srv.hasMaxBodySize = true;
        }

        // location hérite de server si non défini
        for (size_t li = 0; li < srv.locations.size(); ++li)
        {
            LocationConfig& loc = srv.locations[li];

            if (!loc.hasKeepalive && srv.hasKeepalive)
            {
                loc.keepaliveTimeoutSec = srv.keepaliveTimeoutSec;
                loc.hasKeepalive = true;
            }
            if (!loc.hasMaxBodySize && srv.hasMaxBodySize)
            {
                loc.maxBodySize = srv.maxBodySize;
                loc.hasMaxBodySize = true;
            }
        }
    }
}
/*LocationConfig Parser::parseFirstLocationInFile()
{
	// cherche "location"
	while (isNotEnd())
	{
		if (checkType(WORD) && _toks[_pos].wordText == "location")
		{
			consume(WORD);              // consomme "location"
			return parseLocationBlock(); // parse le bloc (préfixe + ...)
		}
		_pos++;
	}
	throw std::invalid_argument("no 'location' found");
}*/
/////////////////////////////////////////////////////////////////////////////

/*static int effectiveKeepaliveTimeoutSec(const HttpConfig& http, const ServerConfig& srv, const LocationConfig& loc)
{
	if (loc.hasKeepalive)
		return loc.keepaliveTimeoutSec;
	if (srv.hasKeepalive)
		return srv.keepaliveTimeoutSec;
	return http.keepaliveTimeoutSec;
}

static std::size_t effectiveBodySize(const HttpConfig& http, const ServerConfig& srv, const LocationConfig& loc)
{
	if (loc.hasMaxBodySize)
		return loc.maxBodySize;
	if (srv.hasMaxBodySize)
		return srv.maxBodySize;
	return http.maxBodySize;
}*/

HttpConfig Parser::parseConfig()
{
	HttpConfig http;
	if (!isNotEnd())
		throw std::invalid_argument("webserv: [emerg] no configuration found");
	if (!checkType(WORD))
		throwUnexpectedToken(currentToken());
	const Token& topTok = consume(WORD);
	if (topTok.wordText == "server" || topTok.wordText == "location")
		throwDirectiveNotAllowedHere(topTok);
	if (topTok.wordText != "http")
		throwUnknownDirective(topTok);
	http = parseHttpBlock(topTok);
	if (isNotEnd())
		throwUnexpectedToken(currentToken());
	applyEffectifData(http);
	return http;
}
