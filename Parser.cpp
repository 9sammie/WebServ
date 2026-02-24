#include "Token.hpp"
#include "Config.hpp"

#include <vector>
#include <string>
#include <exception>
#include <sstream>
#include <cstdlib>   // strtol
#include <cerrno>    // errno
#include <climits>   // INT_MAX
#include <cctype>    // isdigit

static bool isNotEnd(const displayState& ds)
{
	if (ds.pos >= ds.toks.size())
		return false;
	return true;
}

static bool checkType(const displayState& ds, TokenType expectedType)
{
	if (!isNotEnd(ds))
		return false;
	if (ds.toks[ds.pos].type == expectedType)
		return true;
	return false;
}
		
static const Token& consume(displayState& ds, TokenType expectedType)
{
	if (!isNotEnd(ds))
		throw std::exception("Fin de fichier");
	if (!checkType(ds, expectedType))
		throw std::exception("Le type de token attendu n est pas correcte");
	ds.pos++;
	return ds.toks[ds.pos - 1]; //on retourne par ref&
}

static std::vector<std::string> readDirectiveArgs(displayState& ds)
{
	std::vector<std::string> args;
	while (isNotEnd(ds) && checkType(ds, WORD))
	{
		Token t = consume(ds, WORD);
		args.push_back(t.wordText);
	}
	consume(ds, SEMICOLON);
	return args;
}

static bool parseOnOffArg(const std::vector<std::string>& args, const std::string& name)
{
    if (args.size() != 1)
        throw std::exception("expects exactly 1 argument");
	if (args[0] == "on")
		return true;
    if (args[0] == "off")
        return false;
	throw std::exception("on ou off sont uniquement acceptes");
}

static void parseLocationDirective(displayState& ds, LocationConfig& loc, const std::string& name)
{
	if (name.empty())
		throw std::exception("Ne peux pas etre vide");
	if (name == "root")
	{
		std::vector<std::string> args = readDirectiveArgs(ds);
		if (args.size() != 1)
			throw std::exception ("Seul arg accepte");
		loc.root = args[0];
	}
	else if (name == "index")
	{
		std::vector<std::string> args = readDirectiveArgs(ds);
		if (args.size() != 1)
			throw std::exception("Ne peux pas etre vide");
		loc.index = args[0];
	}
	else if (name == "autoindex")
	{
		std::vector<std::string> args = readDirectiveArgs(ds);
		loc.autoindex = parseOnOffArg(args, name);
	}
	else
		throw std::exception("directive inconnue dans le bloc location");
}
	
static LocationConfig parseLocationBlock(ParseState& ps, int locationLine)
{
    (void)locationLine; // si pas utilisé tout de suite

    LocationConfig loc;

    // ex: location /images {
    int prefixLine = 0;
    loc.prefix = consumeWord(ps, "expected location prefix after 'location'", &prefixLine);

    consume(ps, LBRACE, "expected '{' after location prefix");

    while (!check(ps, RBRACE))
    {
        if (atEnd(ps))
            throw errorLine(prefixLine, "missing '}' to close location block");

        int line = 0;
        std::string name = consumeWord(ps, "expected directive name inside location block", &line);

        if (name == "location")
            throw errorLine(line, "nested 'location' block is not allowed");

        if (name == "server")
            throw errorLine(line, "'server' block is not allowed inside location");

        parseLocationDirective(ps, loc, name, line);
    }

    consume(ps, RBRACE, "expected '}' to close location block");
    return loc;
}




























