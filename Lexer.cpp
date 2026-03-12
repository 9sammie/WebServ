/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 13:00:35 by vakozhev          #+#    #+#             */
/*   Updated: 2026/03/12 13:22:16 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

//#include "Parser.hpp"
#include "Lexer.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <iostream>
#include <algorithm>

//void lexLine(const std::string& str, int line, std::vector<Token>& res);
//static bool isWhitespace(char c);
//static bool isDelim(char c);
//static bool isBlankLine(const std::string& str);
//static void skipComment(std::string& str);

Lexer::Lexer() : _buf("") {}

Lexer::Lexer(const Lexer& src) : _buf(src._buf) {}

Lexer::~Lexer() {}

Lexer& Lexer::operator=(const Lexer& src)
{
	if (this != &src)
		this->_buf = src._buf;
	return *this;
}

bool Lexer::isIncludeDirective(const std::vector<Token>& in, size_t i) const
{
	if (i < in.size() && in[i].type == WORD && in[i].wordText == "include")
		return true;
	else
		return false;
}

void Lexer::validateIncludeDirective(const std::vector<Token>& in, size_t i) const
{
	if (i + 2 >= in.size())
		throw std::runtime_error("invalid include directive (expected: include <file> ; )");
	if (in[i + 1].type != WORD)
		throw std::runtime_error("invalid include directive (expected include path as WORD)");
	if (in[i + 2].type != SEMICOLON)
		throw std::runtime_error("invalid include directive ( missing ';')");
}

std::string Lexer::resolveIncludedFile(const std::vector<Token>& in, size_t i, const std::string& currentFile) const
{
	// d abord il faut appeler validateIncludeDirective
	const std::string& rawPath = in[i + 1].wordText;
	return resolveIncludePath(rawPath, currentFile);
} 

std::string Lexer::dirnameOf(const std::string& path)
{
	std::string::size_type pos = path.rfind('/');
	if (pos == std::string::npos)
		return "."; //pas de / donc dossier courant
	if (pos == 0)
		return "/"; //racine
	return path.substr(0, pos);
}

std::string Lexer::resolveIncludePath(const std::string& inc, const std::string& currentFile)
{
	if (!inc.empty() && inc[0] == '/')
		return inc; //absolu
	std::string dir = dirnameOf(currentFile);
	return dir + "/" + inc;
}

std::vector<Token> Lexer::expandIncludes(const std::vector<Token>& in,
                                        const std::string& currentFile,
                                        std::vector<std::string>& includeStack,
                                        int depth)
{
    if (depth > 5)
        throw std::runtime_error("include depth too large (possible include loop): " + currentFile);

    if (std::find(includeStack.begin(), includeStack.end(), currentFile) != includeStack.end())
        throw std::runtime_error("include loop detected with file: " + currentFile);

    includeStack.push_back(currentFile);

    std::vector<Token> out;
    out.reserve(in.size());

    for (size_t i = 0; i < in.size(); )
    {
		if (isIncludeDirective(in, i))
		{
			validateIncludeDirective(in, i);
			std::string incPath = resolveIncludedFile(in, i, currentFile);
			std::vector<Token> incRaw = lexFileRaw(incPath);
			std::vector<Token> incExpanded = expandIncludes(incRaw, incPath, includeStack, depth = 1);
			out.insert(out.end(), incExpanded.begin(), incExpanded.end());
			i = i + 3;
			continue;
		}
		out.push_back(in[i]);
		++i;
	}
	includeStack.pop_back();
	return out;
}	

std::vector<Token> Lexer::lexFile(const std::string& path)
{
	std::vector<std::string> stack; //liste tous les includes 
	std::vector<Token> raw = lexFileRaw(path);
	return expandIncludes(raw, path, stack, 0); //retourne la version ou tous les includes sont remplaces
}

std::vector<Token> Lexer::lexFileRaw(const std::string& path)
{
    std::ifstream infile(path.c_str());//ouvrir le fichier
    if (!infile.is_open())
        throw std::runtime_error("cannot open file: " + path);
    std::vector<Token> res;
    int line = 1;
    while (std::getline(infile, _buf))//parcourir ligne par ligne
    {
        skipComment(_buf);//enlever des commentaires
        if (isBlankLine(_buf)) {//ignorer les lignes vides
            ++line;
            continue;
        }

        lexLine(_buf, line, res); //decouper la ligne en tokens
        ++line;
    }
	
    if (res.empty())
        throw std::runtime_error("empty or comment only " + path);

    return res;//retourne des tokens en vector
}

void Lexer::lexLine(const std::string& str, int line, std::vector<Token>& res)
{
	size_t i = 0;
	while (i < str.size())
	{
		while (i < str.size() && isWhitespace(str[i]))
			++i;
		if (i >= str.size())
			return;
		if (str[i] == '{')
		{
			res.push_back(Token(LBRACE, "{", line));
			++i;
			continue;
		}
		if (str[i] == '}')
		{
			res.push_back(Token(RBRACE, "}", line));
			++i;
			continue;
		}
		if (str[i] == ';')
		{
			res.push_back(Token(SEMICOLON, ";", line));
			++i;
			continue;
		}
		size_t start = i;
		while (i < str.size() && !isWhitespace(str[i]) && !isDelim(str[i]) && str[i] != '#')
			++i;
		if (i > start)
			res.push_back(Token(WORD, str.substr(start, i - start), line));
	}
}
		
bool Lexer::isWhitespace(char c)
{
	if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
		return true;
	return false;
}

bool Lexer::isDelim(char c)
{
	if (c == '{' || c == '}' || c == ';')
		return true;
	return false;
}

bool Lexer::isBlankLine(const std::string& str)
{
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (!isWhitespace(str[i]))
			return false;
	}
	return true;
}

void Lexer::skipComment(std::string& str)
{
	size_t pos = str.find('#'); // pos recoit l index ou se trouve #
	if (pos != std::string::npos)
		str.erase(pos); // supprime tout depuis #
}
//////////////////////////////////////////////////////////
ListenConfig Parser::parseListenArg(const std::string& s)
{
    ListenConfig lc;
    lc.host = "";
    lc.port = 0;

    // Cherche ':'
    size_t pos = s.find(':');
    if (pos == std::string::npos)
    {
        // seulement port
        lc.port = parsePort(s);
        return lc;
    }

    // host:port
    std::string hostPart = s.substr(0, pos);
    std::string portPart = s.substr(pos + 1);

    if (hostPart.empty())
        throw std::invalid_argument("listen host is empty");

    lc.host = hostPart;
    lc.port = parsePort(portPart);
    return lc;
}

int Parser::parsePort(const std::string& s)
{
    int port = parsePositiveInt_strtol(s, "listen port");
    if (port < 1 || port > 65535)
        throw std::invalid_argument("listen port must be between 1 and 65535");
    return port;
}
/*static const char* tokenTypeName(TokenType t)
{
	switch (t)
	{
		case WORD:
			return "WORD";
		case LBRACE:
			return "LBRACE";
		case RBRACE:
			return "RBRACE";
		case SEMICOLON:
			return "SEMICOLON";
		case COMMENT:
			return "COMMENT";
	}
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_path>\n";
		return 1;
	}
	try
	{
		Lexer lx;
		std::vector<Token> toks = lx.lexFile(argv[1]);
		//ParseState ps(toks);
		//std::cout << "ParseState.pos = " << ps.pos << "\n";
		//std::cout << "ParseState.toks.size() = " << ps.toks.size() << "\n";

		//if (!ps.toks.empty())
		//{
    	//	std::cout << "Current token at pos 0: "
          //    		  << tokenTypeName(ps.toks[ps.pos].type)
            //  		  << " \"" << ps.toks[ps.pos].wordText << "\""
              //       << " line " << ps.toks[ps.pos].line << "\n";
		//}
		for (size_t i = 0; i < toks.size(); ++i)
		{
			std::cout << toks[i].line << "  "
          			  << tokenTypeName(toks[i].type) << "  "
            		  << "\"" << toks[i].wordText << "\"\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }
    return 0;
}*/
