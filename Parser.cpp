#include "Token.hpp"
#include "Config.hpp"
#include "Parser.hpp"
#include <vector>
#include <string>
#include <stdexcept>

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
		throw std::invalid_argument("Fin de fichier");
	if (!checkType(ds, expectedType))
		throw std::invalid_argument("Le type de token attendu n est pas correcte");
	ds.pos++;
	return ds.toks[ds.pos - 1]; //on retourne par ref&
}

static std::vector<std::string> readDirectiveArgs(displayState& ds)
{
	std::vector<std::string> args;
	while (isNotEnd(ds) && checkType(ds, WORD))
	{
		const Token& t = consume(ds, WORD);
		args.push_back(t.wordText);
	}
	consume(ds, SEMICOLON);
	return args;
}

static bool parseOnOffArg(const std::vector<std::string>& args)
{
	if (args.size() != 1)
		throw std::invalid_argument("expects exactly 1 argument");
	if (args[0] == "on")
		return true;
	if (args[0] == "off")
		return false;
	throw std::invalid_argument("on ou off sont uniquement acceptes");
}

static void parseLocationDirective(displayState& ds, LocationConfig& loc, const std::string& name) //int line garder ?
{
	if (name.empty())
		throw std::invalid_argument("Ne peux pas etre vide");
	if (name == "root")
	{
		std::vector<std::string> args = readDirectiveArgs(ds);
		if (args.size() != 1)
			throw std::invalid_argument("Seul arg accepte");
		loc.root = args[0];
	}
	else if (name == "index")
	{
		std::vector<std::string> args = readDirectiveArgs(ds);
		if (args.size() != 1)
			throw std::invalid_argument("Seul arg accepte");
		loc.index = args[0];
	}
	else if (name == "autoindex")
	{
		std::vector<std::string> args = readDirectiveArgs(ds);
		loc.autoindex = parseOnOffArg(args);
	}
	else
		throw std::invalid_argument("directive inconnue dans le bloc location");
}
	
LocationConfig parseLocationBlock(displayState& ds)
{
    LocationConfig loc;
	if (!checkType(ds, WORD))
		throw std::invalid_argument("expected location prefix after 'location'");
	const Token& prefixTok = consume(ds, WORD); 
    loc.prefix = prefixTok.wordText;
    consume(ds, LBRACE);
    while (!checkType(ds, RBRACE))
    {
        if (!isNotEnd(ds))
            throw std::invalid_argument("missing '}' to close location block");
		if (!checkType(ds, WORD))
			throw std::invalid_argument("expected directive name inside location block");
        const Token& nameTok = consume(ds, WORD);
		const std::string name = nameTok.wordText;

        if (name == "location")
            throw std::invalid_argument("nested 'location' block is not allowed");

        if (name == "server")
            throw std::invalid_argument("'server' block is not allowed inside location");

        parseLocationDirective(ds, loc, name);
    }

    consume(ds, RBRACE);
    return loc;
}




























