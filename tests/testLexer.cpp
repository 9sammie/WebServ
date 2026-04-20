/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testLexer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 12:43:59 by vakozhev          #+#    #+#             */
/*   Updated: 2026/03/16 14:39:35 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "testsHelper.hpp"
#include "Lexer.hpp"
#include "Token.hpp"
#include <vector>
#include <stdexcept>
#include <string>

static void assertToken(const Token& tok, int expectedType, const std::string& expectedText, int expectedLine)
{
	assertEqualInt(expectedType, tok.type, "wrong token type");
	assertEqualString(expectedText, tok.wordText, "wron token text");
	assertEqualInt(expectedLine, tok.line, "wrong token line");
}

static void lexerValidMinimalConf()
{
	printTestHeader("lexer valid minimal config");
	Lexer lexer;
	std::vector<Token> tokens = lexer.lexFile("tests/configs/valid.conf");
	assertEqualInt(9, tokens.size(), "valid.conf should contain 9 tokens");
	assertToken(tokens[0], WORD, "http", 1);
	assertToken(tokens[1], LBRACE, "{", 1);
	assertToken(tokens[2], WORD, "server", 2);
	assertToken(tokens[3], LBRACE, "{", 2);
	assertToken(tokens[4], WORD, "listen", 3);
	assertToken(tokens[5], WORD, "127.0.0.1:8080", 3);
	assertToken(tokens[6], SEMICOLON, ";", 3);
	assertToken(tokens[7], RBRACE, "}", 4);
	assertToken(tokens[8], RBRACE, "}", 5);
	printTestOK("lexer valid minimal conf");
}

static void lexerIgnoresComments()
{
	printTestHeader("lexer ignores comments");
	Lexer lexer;
	std::vector<Token> tokens = lexer.lexFile("tests/configs/withComments.conf");
	assertTrue(tokens.size() > 0, "lexer should return tokens");
	assertToken(tokens[0], WORD, "server", 2);
	assertToken(tokens[1], LBRACE, "{", 2);
	assertToken(tokens[2], WORD, "listen", 3);
	assertToken(tokens[3], WORD, "8080", 3);
	assertToken(tokens[4], SEMICOLON, ";", 3);
	assertToken(tokens[5], RBRACE, "}", 4);
	printTestOK("lexer ignores comments");
}

static void lexerRejectsMissingConf()
{
	printTestHeader("lexer rejects missing conf");
	Lexer lexer;
	bool exceptionCatched = false;
	try
	{
		lexer.lexFile("tests/configs/doesNotExist.conf");
	}
	catch (const std::exception&)
	{
		exceptionCatched = true;
	}
	assertTrue(exceptionCatched, "lexer should throw on missing conf");
	printTestOK("lexer rejects missing conf");
}

static void lexerRejectsEmptyConf()
{
	printTestHeader("lexer rejects empty conf");
	Lexer lexer;
	bool exceptionCatched = false;
	try
	{
		lexer.lexFile("tests/configs/empty.conf");
	}
	catch (const std::exception&)
	{
		exceptionCatched = true;
	}
	assertTrue(exceptionCatched, "lexer should throw on empty conf");
	printTestOK("lexer rejects empty conf");
}

static void lexerRejectsCommentOnlyConf()
{
	printTestHeader("lexer rejects comment only conf");
	Lexer lexer;
	bool exceptionCatched = false;
	try
	{
		lexer.lexFile("tests/configs/commentOnly.conf");
	}
	catch (const std::exception&)
	{
		exceptionCatched = true;
	}
	assertTrue(exceptionCatched, "lexer should throw on comment only conf");
	printTestOK("lexer rejects comment only conf");
}

static void lexerRejectsBlankOnlyConf()
{
	printTestHeader("lexer rejects blank only conf");
	Lexer lexer;
	bool exceptionCatched = false;
	try
	{
		lexer.lexFile("tests/configs/blankOnly.conf");
	}
	catch (const std::exception&)
	{
		exceptionCatched = true;
	}
	assertTrue(exceptionCatched, "lexer should throw on blank only conf");
	printTestOK("lexer rejects blank only conf");
}

void runLexerTests()
{
	lexerValidMinimalConf();
	lexerIgnoresComments();
	lexerRejectsMissingConf();
	lexerRejectsEmptyConf();
	lexerRejectsCommentOnlyConf();
	lexerRejectsBlankOnlyConf();
}
