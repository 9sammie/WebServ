/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 09:24:15 by vakozhev          #+#    #+#             */
/*   Updated: 2026/03/13 14:05:06 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include <stdexcept>
#include <cstdlib> //strtol, strtoull
#include <cerrno>
#include <limits>

Parser::Parser(const std::vector<Token>& toks) : _toks(toks), _pos(0)
{}

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
		throw std::invalid_argument("Fin de fichier");
	if (!checkType(expectedType))
		throw std::invalid_argument("Le type de token attendu n est pas correcte");
	_pos++;
	return _toks[_pos - 1]; //on retourne par ref&
}

bool Parser::checkWord(const std::string& expected) const //il s agit d un MOT et c est un mot que j attends
{
	if (checkType(WORD) && _toks[_pos].wordText == expected)
		return true;
    return false;
}

void Parser::consumeWord(const std::string& expected)
{
	if (!checkWord(expected))
		throw std::invalid_argument("expected word: " + expected);
	consume(WORD);
}

void Parser::skipBlock()
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
}

std::vector<std::string> Parser::readDirectiveArgs()
{
	std::vector<std::string> args;
	while (isNotEnd() && checkType(WORD))
	{
		const Token& t = consume(WORD);
		args.push_back(t.wordText);
	}
	consume(SEMICOLON);
	return args;
}

bool Parser::parseOnOffArg(const std::vector<std::string>& args)
{
	if (args.size() != 1)
		throw std::invalid_argument("expects exactly 1 argument");
	if (args[0] == "on")
		return true;
	if (args[0] == "off")
		return false;
	throw std::invalid_argument("on ou off sont uniquement acceptes");
}

int Parser::parsePositiveInt(const std::string& s, const std::string& name)
{
	if (s.empty())
		throw std::invalid_argument(name + " must be a number");
	char* end = NULL;
	errno = 0;
	long res = std::strtol(s.c_str(), &end, 10);//&end = l adresse de end pour ecrire dedans la valeur
	if (errno != 0 || end == s.c_str() || *end != '\0') //owerflow/underflow, ou aucun chiffre, ou il reste des car 12a p ex
		throw std::invalid_argument(name + " must be a number");
	if (res < 0)
		throw std::invalid_argument(name + " must be >= 0"); //positifs
	if (res > std::numeric_limits<int>::max())
		throw std::invalid_argument(name + " is too large"); //doit rentrer dans int
	return static_cast<int>(res);
}	

std::size_t Parser::parseSizeT(const std::string& s, const std::string& name)
{
	if (s.empty())
		throw std::invalid_argument(name + " must be a number");
	if (s[0] == '-')
		throw std::invalid_argument(name + " must be >= 0");
	char* end = NULL;
	errno = 0;
	unsigned long long res = std::strtoull(s.c_str(), &end, 10);
	if (errno != 0 || end == s.c_str() || *end != '\0')
		throw std::invalid_argument(name + " must be a number");
	if (res > static_cast<unsigned long long>(std::numeric_limits<size_t>::max()))
		throw std::invalid_argument(name + " is too large");
	return static_cast<size_t>(res);
} 

int Parser::parsePort(const std::string& s)
{
	int port = parsePositiveInt(s, "listen port");
	if (port < 1 || port > 65535)
		throw std::invalid_argument("listen port must be between 1 and 65535");
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

void Parser::applyMethods(LocationConfig& loc, const std::vector<std::string>& args)
{
	if (args.empty())
		throw std::invalid_argument("methods expects at least 1 argument");
	loc.methods.clear();
	for (std::size_t i = 0; i < args.size(); ++i)
	{
		std::string m = args[i];
		if (!isHttpMethod(m))
			throw std::invalid_argument("unknown HTTP method: " + m);
		if (containsMethod(loc.methods, m))
			throw std::invalid_argument("duplicate method: " + m);
		loc.methods.push_back(m);
	}
	loc.uploadAuthorised = containsMethod(loc.methods, "POST");
}

void Parser::parseErrorPage(std::map<int, std::string>& errors, const std::vector<std::string>& args, const std::string& name)
{
	if (args.size() < 2)
		throw std::invalid_argument("error_page expects: <code...> <path> in " + name);
	const std::string& path = args[args.size() - 1];
	for (std::size_t i = 0; i + 1 < args.size(); ++i)
	{
		int code = parsePositiveInt(args[i], "error_page code");
		if (code < 300 || code > 599)
			throw std::invalid_argument("error_page code must be between 300 and 599");
		errors[code] = path;
	}
}

void Parser::parseLocationDirective(LocationConfig& loc, const std::string& name) //int line garder ?
{
	if (name.empty())
		throw std::invalid_argument("Ne peux pas etre vide");
	if (name == "root")
	{
		std::vector<std::string> args = readDirectiveArgs();
		if (args.size() != 1)
			throw std::invalid_argument("Seul arg accepte");
		loc.root = args[0];
		return;
	}
	if (name == "index")
	{
		std::vector<std::string> args = readDirectiveArgs();
		if (args.size() != 1)
			throw std::invalid_argument("Seul arg accepte");
		loc.index = args[0];
		return;
	}
	if (name == "autoindex")
	{
		std::vector<std::string> args = readDirectiveArgs();
		loc.autoindex = parseOnOffArg(args);
		return;
	}
	if (name == "methods")
	{
		std::vector<std::string> args = readDirectiveArgs();
		applyMethods(loc, args);
		return;
	}
	if (name == "return")
	{
		std::vector<std::string> args = readDirectiveArgs();
		if (args.size() != 2)
			throw std::invalid_argument("return expects: <code> <target>");
		int code = parsePositiveInt(args[0], "return code");
		if (code != 301 && code != 302 && code != 307 && code != 308)
			throw std::invalid_argument("return code must be one of 301, 302, 307, 308");
		loc.hasRedirection = true;
		loc.redirectCode = code;
		loc.redirectTarget = args[1];
		return;
	}
	if (name == "client_max_body_size")
	{
		std::vector<std::string> args = readDirectiveArgs();
		if (args.size() != 1)
			throw std::invalid_argument("client_max_body_size expects exactly 1 argument");
		loc.maxBodySize = parseSizeT(args[0], "client_max_body_size");
		loc.hasMaxBodySize = true;
		return;
	}
	if (name == "cgi_ext")
	{
		std::vector<std::string> args = readDirectiveArgs();
		if (args.size() != 1)
			throw std::invalid_argument("cgi_ext expects exactly 1 argument");
		loc.cgiExt = args[0];
		loc.cgiAuthorised = true;
		return;
	}
	if (name == "cgi_path")
	{
		std::vector<std::string> args = readDirectiveArgs();
		if (args.size() != 1)
			throw std::invalid_argument("cgi_path expects exactly 1 argument");
		loc.cgiPath = args[0];
		loc.cgiAuthorised = true;
		return;
	}
	throw std::invalid_argument("directive inconnue dans le bloc location");
}
	
LocationConfig Parser::parseLocationBlock()
{
    LocationConfig loc;
	if (!checkType(WORD))
		throw std::invalid_argument("expected location prefix after 'location'");
	const Token& prefixTok = consume(WORD); 
    loc.prefix = prefixTok.wordText;
    consume(LBRACE);
    while (!checkType(RBRACE))
    {
        if (!isNotEnd())
            throw std::invalid_argument("missing '}' to close location block");
		if (!checkType(WORD))
			throw std::invalid_argument("expected directive name inside location block");
        const Token& nameTok = consume(WORD);
		const std::string name = nameTok.wordText;

        if (name == "location")
            throw std::invalid_argument("nested 'location' block is not allowed");

        if (name == "server")
            throw std::invalid_argument("'server' block is not allowed inside location");

        parseLocationDirective(loc, name);
    }
    consume(RBRACE);
    return loc;
}

void Parser::parseHttpDirective(HttpConfig& http, const std::string& name)
{
	if (name == "keepalive_timeout")
	{
		std::vector<std::string> args = readDirectiveArgs();
		if (args.size() != 1)
			throw std::invalid_argument("keepalive_timeout expects exactly 1 argument");
		http.keepaliveTimeoutSec = parsePositiveInt(args[0], "keepalive_timeout");
		return;
	}
	if (name == "client_max_body_size")
	{
		std::vector<std::string> args = readDirectiveArgs();
		if (args.size() != 1)
			throw std::invalid_argument("client_max_body_size expects exactly 1 argument");
		http.maxBodySize = parseSizeT(args[0], "client_max_body_size");
		return;
	}
	if (name == "error_page")
	{
		std::vector<std::string> args = readDirectiveArgs();
		if (args.size() < 2)
			throw std::invalid_argument("error_page expects: <code..> <path>");
		const std::string& path = args[args.size() - 1];
		for (size_t i = 0; i + 1 < args.size(); ++i)
		{
			int code = parsePositiveInt(args[i], "error_page code");
			if (code < 300 || code > 599)
				throw std::invalid_argument("error_page code must be between 300 and 599");
			http.errors[code] = path;
		}
		return;
	}
	throw std::invalid_argument("unknown directive in http block: " + name);
}

HttpConfig Parser::parseHttpBlock()
{
	HttpConfig http;
	consume(LBRACE);
	while (!checkType(RBRACE))
	{
		if (!isNotEnd())
			throw std::invalid_argument("missing '}' to close http block");
		if (!checkType(WORD))
			throw std::invalid_argument("expected directive or block name inside http");
		std::string name = consume(WORD).wordText;
		if (name == "server")
		{
			//skipBlock();
			ServerConfig srv = parseServerBlock();
			http.servers.push_back(ServerConfig(srv));
			continue;
		}
		if (name == "location")
			throw std::invalid_argument("'location' is not allowed directly inside http");
		parseHttpDirective(http, name);
	}
	consume(RBRACE);
	return http;
}

ListenConfig Parser::parseListenArg(const std::string& s)
{
	ListenConfig lc;
	lc.host = "";
	lc.port = 0;
	std::size_t pos = s.find(':');//on peut ecrire listen 127.0.0.1:8080; ou listen 8080;
	if (pos == std::string::npos)
	{
		lc.port = parsePort(s);
		return lc;
	}
	std::string hostPart = s.substr(0, pos);
	std::string portPart = s.substr(pos + 1);
	if (hostPart.empty())
		throw std::invalid_argument("listen host is empty");
	lc.host = hostPart;
	lc.port = parsePort(portPart);
	return lc;
}

void Parser::parseServerDirective(ServerConfig& srv, const std::string& name)
{
	if (name == "listen")
	{
		std::vector<std::string> args = readDirectiveArgs();
		if (args.size() != 1)
			throw std::invalid_argument("listen expects exactly 1 argument");
		ListenConfig lc = parseListenArg(args[0]);
		srv.listens.push_back(lc);
		return;
	}
	if (name == "server_name")
	{
		std::vector<std::string> args = readDirectiveArgs();
		if (args.size() != 1)
			throw std::invalid_argument("server_name expects exactly 1 argument");
		srv.serverName = args[0];
		return;
	}
	if (name == "keepalive_timeout")
	{
		std::vector<std::string> args = readDirectiveArgs();
		if (args.size() != 1)
			throw std::invalid_argument("keepalive_timeout expects exactly 1 argument");
		srv.keepaliveTimeoutSec = parsePositiveInt(args[0], "keepalive_timeout");
		return;
	}
	if (name == "client_max_body_size")
	{
		std::vector<std::string> args = readDirectiveArgs();
		if (args.size() != 1)
			throw std::invalid_argument("client_max_body_size expects exactly 1 argument");
		srv.maxBodySize = parseSizeT(args[0], "client_max_body_size");
		srv.maxBodySize = true;
		return;
	}
	if (name == "error_page")
	{
		std::vector<std::string> args = readDirectiveArgs();
		parseErrorPage(srv.errors, args, "server");
		return;
	}
	throw std::invalid_argument("unknown directive in server block: " + name);
}
		
ServerConfig Parser::parseServerBlock()
{
	ServerConfig srv;
	consume(LBRACE);
	while(!checkType(RBRACE))
	{
		if (!isNotEnd())
			throw std::invalid_argument("missing '}' to close server block");
		if (!checkType(WORD))
			throw std::invalid_argument("expected directive name or 'location' inside server block");
		std::string name = consume(WORD).wordText;
		if (name == "server")
			throw std::invalid_argument("'server' block is not allowed");
		if (name == "http")
			throw std::invalid_argument("'http' block is not allowed inside server");
		if (name == "location")
		{
			LocationConfig loc = parseLocationBlock();
			srv.locations.push_back(loc);
			continue;
		}
		parseServerDirective(srv, name);
	}
	consume(RBRACE);
	if (srv.listens.empty())
		throw std::invalid_argument("server must have at least one listen directive");
	return srv;
}

/////////////////////////////////func will be modified///////////////
void Parser::applyEffectifData(HttpConfig& http)
{
    for (size_t si = 0; si < http.servers.size(); ++si)
    {
        ServerConfig& srv = http.servers[si];

        // server hérite de http si non défini
        if (!srv.hasKeepalive)
        {
            srv.keepaliveTimeoutSec = http.keepaliveTimeoutSec;
            srv.hasKeepalive = true;
        }
        if (!srv.hasMaxBodySize)
        {
            srv.maxBodySize = http.maxBodySize;
            srv.hasMaxBodySize = true;
        }

        // location hérite de server si non défini
        for (size_t li = 0; li < srv.locations.size(); ++li)
        {
            LocationConfig& loc = srv.locations[li];

            if (!loc.hasKeepalive)
            {
                loc.keepaliveTimeoutSec = srv.keepaliveTimeoutSec;
                loc.hasKeepalive = true;
            }
            if (!loc.hasMaxBodySize)
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
	{
		throw std::invalid_argument("empty config");
	}
	if (checkWord("server"))
	{
		throw std::invalid_argument("'server' is not allowed at top-level");
	}
    if (checkWord("location"))
	{
        throw std::invalid_argument("'location' is not allowed at top-level (must be inside server)");
	}
	if (!checkWord("http"))
	{
		throw std::invalid_argument("expected 'http' at top-level");
	}
	consume(WORD);
	http = parseHttpBlock();
	applyEffectifData(http);
	return http;
}

