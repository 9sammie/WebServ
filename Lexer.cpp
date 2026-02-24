/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 13:00:35 by vakozhev          #+#    #+#             */
/*   Updated: 2026/02/24 11:19:33 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Token.hpp"
#include "Parser.hpp"
#include <string> // std::find 
#include <fstream>
#include <iostream>
#include <vector>
#include <stdexcept>

void lexLine(const std::string& str, int line, std::vector<Token>& res);
static bool isWhitespace(char c);
static bool isDelim(char c);
static bool isBlankLine(const std::string& str);
static void skipComment(std::string& str);


std::vector<Token> lexFile(const std::string& path)
{
    std::ifstream infile(path.c_str());
    if (!infile.is_open())
        throw std::runtime_error("cannot open file: " + path);

    std::vector<Token> res;
    std::string str;
    int line = 1;

    while (std::getline(infile, str))
    {
        skipComment(str);

        if (isBlankLine(str)) {
            ++line;
            continue;
        }

        lexLine(str, line, res);
        ++line;
    }
	
    if (res.empty())
        throw std::runtime_error("empty or comment only " + path);

    return res;
}

void lexLine(const std::string& str, int line, std::vector<Token>& res)
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
		res.push_back(Token(WORD, str.substr(start, i - start), line));
	}
}
		
static bool isWhitespace(char c)
{
	if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
		return true;
	return false;
}

static bool isDelim(char c)
{
	if (c == '{' || c == '}' || c == ';')
		return true;
	return false;
}

static bool isBlankLine(const std::string& str)
{
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (!isWhitespace(str[i]))
			return false;
	}
	return true;
}

static void skipComment(std::string& str)
{
	size_t pos = str.find('#'); // pos recoit l index ou se trouve #
	if (pos != std::string::npos)
		str.erase(pos); // supprime tout depuis #
}

static const char* tokenTypeName(TokenType t)
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
		std::vector<Token> toks = lexFile(argv[1]);
		ParseState ps(toks);
		std::cout << "ParseState.pos = " << ps.pos << "\n";
		std::cout << "ParseState.toks.size() = " << ps.toks.size() << "\n";

		if (!ps.toks.empty())
		{
    		std::cout << "Current token at pos 0: "
              		  << tokenTypeName(ps.toks[ps.pos].type)
              		  << " \"" << ps.toks[ps.pos].wordText << "\""
                     << " line " << ps.toks[ps.pos].line << "\n";
		}
		//for (size_t i = 0; i < toks.size(); ++i)
		//{
		//	std::cout << toks[i].line << "  "
          //            << tokenTypeName(toks[i].type) << "  "
            //          << "\"" << toks[i].wordText << "\"\n";
        //}
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }
    return 0;
}
