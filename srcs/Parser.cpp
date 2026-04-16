#include "Parser.hpp"

#include <stdexcept>
#include <cstdlib> //strtol, strtoull
#include <cerrno>
#include <limits>
#include <sstream>
#include <climits>

/* ************************************************************************** */
/*                          canonical form + helpers + local funcs                        */
/* ************************************************************************** */

Parser::Parser(const std::vector<Token>& toks) : _toks(toks), _pos(0)
{}

Parser::~Parser()
{}

static bool isCgiLocation(const LocationConfig& loc)
{
	return (!loc.cgiExt.empty() && !loc.cgiPath.empty());
}

static bool hasIncompleteCgiConfig(const LocationConfig& loc)
{
	return (loc.cgiExt.empty() && !loc.cgiPath.empty()) || (!loc.cgiExt.empty() && loc.cgiPath.empty());
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

static int effectiveKeepaliveTimeoutSec(const HttpConfig& http, const ServerConfig& srv)
{
	if (srv.hasKeepalive)
		return srv.keepaliveTimeoutSec;
	return http.keepaliveTimeoutSec;
}

static int defaultCgiTimeoutSec(int keepaliveSec)
{
	if (keepaliveSec - 5 <= 0)
		throw std::invalid_argument("webserv: [emerg] effective \"keepalive_timeout\" must be greater than 5 to define a CGI timeout with a 5-second gap");
	return keepaliveSec - 5;
}

/* ************************************************************************** */
/*                                 errors                                     */
/* ************************************************************************** */

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

/* ************************************************************************** */
/*                                  token navigation                          */
/* ************************************************************************** */

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
	return _toks[_pos - 1];
}

bool Parser::checkWord(const std::string& expected) const
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

/* ************************************************************************** */
/*                                   value parsing                            */
/**************************************************************************** */

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
	long res = std::strtol(s.c_str(), &end, 10);
	if (errno != 0 || end == s.c_str() || *end != '\0')
		throwInvalidValue(directiveTok, s);
	if (res < 0)
		throwInvalidValue(directiveTok, s);
	if (res > std::numeric_limits<int>::max())
		throwInvalidValue(directiveTok, s);
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

static bool isValidHost(const std::string& s)
{
	std::size_t start = 0;
	for (int parts = 0; parts < 4; parts++)
	{
		std::size_t end = s.find('.', start);
		std::string part;
		if (end == std::string::npos)
			part = s.substr(start);
		else
			part = s.substr(start, end - start);
		if (part.empty())
			return false;
		for (std::size_t i = 0; i < part.size(); i++)
		{
			if (!std::isdigit(static_cast<unsigned char>(part[i])))
				return false;
		}
		if (part.size() > 1 && part[0] == '0')
			return false;
		char* conversionEnd = NULL;
		long res = std::strtol(part.c_str(), &conversionEnd, 10);
		if (*conversionEnd != '\0' || res < 0 || res > 255)
			return false;
		if (parts < 3)
		{
			if (end == std::string::npos)
				return false;
			start = end + 1;
		}
		else
		{
			if (end != std::string::npos)
				return false;
		}
	}
	return true;
}

static bool isSameListenValue(const std::vector<ListenConfig>& listens, const ListenConfig& input)
{
	for (std::size_t i = 0; i < listens.size(); ++i)
	{
		if (listens[i].host == input.host && listens[i].port == input.port)
			return true;
	}
	return false;
}

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

ListenConfig Parser::parseListenArg(const Token& directiveTok, const std::string& s)
{
	ListenConfig lc;
	lc.host = "";
	lc.port = 0;
	std::size_t pos = s.find(':');
	if (pos == std::string::npos)
		throwInvalidValue(directiveTok, s);
	std::string hostPart = s.substr(0, pos);
	std::string portPart = s.substr(pos + 1);
	if (hostPart.empty() || portPart.empty()) 
		throwInvalidValue(directiveTok, s);
	if (!isValidHost(hostPart))
		throwInvalidValue(directiveTok, hostPart);
	lc.host = hostPart;
	lc.port = parsePort(directiveTok, portPart);
	return lc;
}

/* ************************************************************************** */
/*                               parsing of directive                         */
/**************************************************************************** */

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
	if (name == "cgiTimeout")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		if (args.size() != 1)
			throwInvalidArgs(nameTok);
		loc.cgiTimeoutSec = parsePositiveInt(nameTok, args[0]);
		if (loc.cgiTimeoutSec <= 0)
			throwInvalidValue(nameTok, args[0]);
		loc.hasCgiTimeout = true;
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

void Parser::parseServerDirective(ServerConfig& srv, const Token& nameTok)
{
	const std::string& name = nameTok.wordText;
	if (name == "listen")
	{
		std::vector<std::string> args = readDirectiveArgs(nameTok);
		if (args.size() != 1)
			throwInvalidArgs(nameTok);
		ListenConfig lc = parseListenArg(nameTok, args[0]);
		if (isSameListenValue(srv.listens, lc))
			throwDuplicateValue(nameTok, args[0]);
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
		parseErrorPage(srv.errors, nameTok, args);
		return;
	}
	throwUnknownDirective(nameTok);
}

/* ************************************************************************** */
/*                                 parsing of blocks                          */
/**************************************************************************** */

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

/* ************************************************************************** */
/*                                  main functions                           */
/*************************************************************************** */

//#include <iostream>
void Parser::applyEffectiveData(HttpConfig& http)
{
	for (std::size_t i = 0; i < http.servers.size(); ++i)
	{
		ServerConfig& srv = http.servers[i];

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

		int srvKeepalive = effectiveKeepaliveTimeoutSec(http, srv);
		//std::cout << "[DEBUG] server[" << i << "] effective keepalive_timeout = "<< srvKeepalive << "s" << std::endl;
		std::size_t cgiLocationCount = 0;

		for (std::size_t j = 0; j < srv.locations.size(); ++j)
		{
			LocationConfig& loc = srv.locations[j];

			if (hasIncompleteCgiConfig(loc))
				throw std::invalid_argument("webserv: [emerg] CGI location requires both \"cgi_ext\" and \"cgi_path\"");

			if (!loc.hasMaxBodySize && srv.hasMaxBodySize)
			{
				loc.maxBodySize = srv.maxBodySize;
				loc.hasMaxBodySize = true;
			}

			bool cgiLoc = isCgiLocation(loc);

			if (cgiLoc)
			{
				++cgiLocationCount;
				if (cgiLocationCount > 1)
					throw std::invalid_argument("webserv: [emerg] only one CGI location is allowed per server");
			}

			if (loc.hasCgiTimeout && !cgiLoc)
				throw std::invalid_argument("webserv: [emerg] \"cgiTimeout\" is only allowed in CGI locations");

			if (cgiLoc)
			{
				if (srvKeepalive - 5 <= 0)
					throw std::invalid_argument("webserv: [emerg] effective \"keepalive_timeout\" must be greater than 5 to allow CGI");

				if (!loc.hasCgiTimeout)
				{
					loc.cgiTimeoutSec = defaultCgiTimeoutSec(srvKeepalive);
					loc.hasCgiTimeout = true;
				}

				if (loc.cgiTimeoutSec > srvKeepalive - 5)
					throw std::invalid_argument("webserv: [emerg] \"cgiTimeout\" must be at least 5 seconds shorter than effective \"keepalive_timeout\"");
				//std::cout << "[DEBUG] location \"" << loc.prefix << "\" effective cgiTimeout = "<< loc.cgiTimeoutSec << "s" << std::endl;
			}
		}
	}
}

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
	applyEffectiveData(http);
	return http;
}
