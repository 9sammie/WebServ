/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 13:00:35 by vakozhev          #+#    #+#             */
/*   Updated: 2026/03/04 15:49:11 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

//#include "Parser.hpp"
#include "Lexer.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <iostream>

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

std::vector<Token> Lexer::lexFile(const std::string& path)
{
    std::ifstream infile(path.c_str());
    if (!infile.is_open())
        throw std::runtime_error("cannot open file: " + path);
    std::vector<Token> res;
    int line = 1;
    while (std::getline(infile, _buf))
    {
        skipComment(_buf);
        if (isBlankLine(_buf)) {
            ++line;
            continue;
        }

        lexLine(_buf, line, res);
        ++line;
    }
	
    if (res.empty())
        throw std::runtime_error("empty or comment only " + path);

    return res;
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
