/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 16:24:00 by vakozhev          #+#    #+#             */
/*   Updated: 2026/04/02 15:25:42 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "testsHelper.hpp"
#include "Parser.hpp"
#include "Token.hpp"
#include "Config.hpp"
#include <vector>
#include <stdexcept>
#include <string>
#include <map>

static Token makeToken(TokenType type, const std::string& text, int line)
{
    Token tok;
    tok.type = type;
    tok.wordText = text;
    tok.line = line;
    return tok;
}

static void parserAcceptsMinimalHttpServer()
{
    printTestHeader("parser accepts minimal http server");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));
    tokens.push_back(makeToken(WORD, "server", 2));
    tokens.push_back(makeToken(LBRACE, "{", 2));
    tokens.push_back(makeToken(WORD, "listen", 3));
    tokens.push_back(makeToken(WORD, "8080", 3));
    tokens.push_back(makeToken(SEMICOLON, ";", 3));
    tokens.push_back(makeToken(RBRACE, "}", 4));
    tokens.push_back(makeToken(RBRACE, "}", 5));

    Parser parser(tokens);
    HttpConfig http = parser.parseConfig();

    assertEqualInt(1, static_cast<int>(http.servers.size()), "http should contain 1 server");
    assertEqualInt(1, static_cast<int>(http.servers[0].listens.size()), "server should contain 1 listen");
    assertEqualInt(8080, http.servers[0].listens[0].port, "listen port should be 8080");

    printTestOK("parser accepts minimal http server");
}

static void parserRejectsEmptyTokenList()
{
    printTestHeader("parser rejects empty token list");

    std::vector<Token> tokens;
    Parser parser(tokens);

    bool exceptionCatched = false;
    try
    {
        parser.parseConfig();
    }
    catch (const std::exception&)
    {
        exceptionCatched = true;
    }

    assertTrue(exceptionCatched, "parser should throw on empty token list");
    printTestOK("parser rejects empty token list");
}

static void parserRejectsUnknownTopLevelDirective()
{
    printTestHeader("parser rejects unknown top level directive");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "kapouet", 1));

    Parser parser(tokens);

    bool exceptionCatched = false;
    try
    {
        parser.parseConfig();
    }
    catch (const std::exception&)
    {
        exceptionCatched = true;
    }

    assertTrue(exceptionCatched, "parser should throw on unknown top level directive");
    printTestOK("parser rejects unknown top level directive");
}

static void parserRejectsServerAtTopLevel()
{
    printTestHeader("parser rejects server at top level");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "server", 1));

    Parser parser(tokens);

    bool exceptionCatched = false;
    try
    {
        parser.parseConfig();
    }
    catch (const std::exception&)
    {
        exceptionCatched = true;
    }

    assertTrue(exceptionCatched, "parser should reject server at top level");
    printTestOK("parser rejects server at top level");
}

static void parserRejectsHttpWithoutOpeningBrace()
{
    printTestHeader("parser rejects http without opening brace");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));

    Parser parser(tokens);

    bool exceptionCatched = false;
    try
    {
        parser.parseConfig();
    }
    catch (const std::exception&)
    {
        exceptionCatched = true;
    }

    assertTrue(exceptionCatched, "parser should reject http block without opening brace");
    printTestOK("parser rejects http without opening brace");
}

static void parserRejectsServerWithoutListen()
{
    printTestHeader("parser rejects server without listen");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));
    tokens.push_back(makeToken(WORD, "server", 2));
    tokens.push_back(makeToken(LBRACE, "{", 2));
    tokens.push_back(makeToken(RBRACE, "}", 3));
    tokens.push_back(makeToken(RBRACE, "}", 4));

    Parser parser(tokens);

    bool exceptionCatched = false;
    try
    {
        parser.parseConfig();
    }
    catch (const std::exception&)
    {
        exceptionCatched = true;
    }

    assertTrue(exceptionCatched, "parser should reject server without listen");
    printTestOK("parser rejects server without listen");
}

static void parserRejectsInvalidListenPort()
{
    printTestHeader("parser rejects invalid listen port");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));
    tokens.push_back(makeToken(WORD, "server", 2));
    tokens.push_back(makeToken(LBRACE, "{", 2));
    tokens.push_back(makeToken(WORD, "listen", 3));
    tokens.push_back(makeToken(WORD, "70000", 3));
    tokens.push_back(makeToken(SEMICOLON, ";", 3));
    tokens.push_back(makeToken(RBRACE, "}", 4));
    tokens.push_back(makeToken(RBRACE, "}", 5));

    Parser parser(tokens);

    bool exceptionCatched = false;
    try
    {
        parser.parseConfig();
    }
    catch (const std::exception&)
    {
        exceptionCatched = true;
    }

    assertTrue(exceptionCatched, "parser should reject invalid listen port");
    printTestOK("parser rejects invalid listen port");
}

static void parserAcceptsHttpKeepaliveTimeout()
{
    printTestHeader("parser accepts http keepalive timeout");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));
    tokens.push_back(makeToken(WORD, "keepalive_timeout", 2));
    tokens.push_back(makeToken(WORD, "15", 2));
    tokens.push_back(makeToken(SEMICOLON, ";", 2));
    tokens.push_back(makeToken(WORD, "server", 3));
    tokens.push_back(makeToken(LBRACE, "{", 3));
    tokens.push_back(makeToken(WORD, "listen", 4));
    tokens.push_back(makeToken(WORD, "8080", 4));
    tokens.push_back(makeToken(SEMICOLON, ";", 4));
    tokens.push_back(makeToken(RBRACE, "}", 5));
    tokens.push_back(makeToken(RBRACE, "}", 6));

    Parser parser(tokens);
    HttpConfig http = parser.parseConfig();

    assertTrue(http.hasKeepalive, "http should have keepalive");
    assertEqualInt(15, http.keepaliveTimeoutSec, "http keepalive timeout should be 15");
    assertTrue(http.servers[0].hasKeepalive, "server should inherit keepalive");
    assertEqualInt(15, http.servers[0].keepaliveTimeoutSec, "server keepalive timeout should be 15");

    printTestOK("parser accepts http keepalive timeout");
}

static void parserAcceptsHttpClientMaxBodySize()
{
    printTestHeader("parser accepts http client max body size");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));
    tokens.push_back(makeToken(WORD, "client_max_body_size", 2));
    tokens.push_back(makeToken(WORD, "1048576", 2));
    tokens.push_back(makeToken(SEMICOLON, ";", 2));
    tokens.push_back(makeToken(WORD, "server", 3));
    tokens.push_back(makeToken(LBRACE, "{", 3));
    tokens.push_back(makeToken(WORD, "listen", 4));
    tokens.push_back(makeToken(WORD, "8080", 4));
    tokens.push_back(makeToken(SEMICOLON, ";", 4));
    tokens.push_back(makeToken(RBRACE, "}", 5));
    tokens.push_back(makeToken(RBRACE, "}", 6));

    Parser parser(tokens);
    HttpConfig http = parser.parseConfig();

    assertTrue(http.hasMaxBodySize, "http should have client_max_body_size");
    assertTrue(http.maxBodySize == 1048576u, "http max body size should be 1048576");
    assertTrue(http.servers[0].hasMaxBodySize, "server should inherit client_max_body_size");
    assertTrue(http.servers[0].maxBodySize == 1048576u, "server max body size should be 1048576");

    printTestOK("parser accepts http client max body size");
}

static void parserAcceptsServerName()
{
    printTestHeader("parser accepts server_name");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));
    tokens.push_back(makeToken(WORD, "server", 2));
    tokens.push_back(makeToken(LBRACE, "{", 2));
    tokens.push_back(makeToken(WORD, "listen", 3));
    tokens.push_back(makeToken(WORD, "8080", 3));
    tokens.push_back(makeToken(SEMICOLON, ";", 3));
    tokens.push_back(makeToken(WORD, "server_name", 4));
    tokens.push_back(makeToken(WORD, "test_serv_1", 4));
    tokens.push_back(makeToken(SEMICOLON, ";", 4));
    tokens.push_back(makeToken(RBRACE, "}", 5));
    tokens.push_back(makeToken(RBRACE, "}", 6));

    Parser parser(tokens);
    HttpConfig http = parser.parseConfig();

    assertEqualString("test_serv_1", http.servers[0].serverName, "wrong server_name");
    printTestOK("parser accepts server_name");
}

static void parserAcceptsLocationRoot()
{
    printTestHeader("parser accepts location /");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));
    tokens.push_back(makeToken(WORD, "server", 2));
    tokens.push_back(makeToken(LBRACE, "{", 2));
    tokens.push_back(makeToken(WORD, "listen", 3));
    tokens.push_back(makeToken(WORD, "8080", 3));
    tokens.push_back(makeToken(SEMICOLON, ";", 3));
    tokens.push_back(makeToken(WORD, "location", 4));
    tokens.push_back(makeToken(WORD, "/", 4));
    tokens.push_back(makeToken(LBRACE, "{", 4));
    tokens.push_back(makeToken(WORD, "root", 5));
    tokens.push_back(makeToken(WORD, "./html/site1", 5));
    tokens.push_back(makeToken(SEMICOLON, ";", 5));
    tokens.push_back(makeToken(WORD, "index", 6));
    tokens.push_back(makeToken(WORD, "home.html", 6));
    tokens.push_back(makeToken(SEMICOLON, ";", 6));
    tokens.push_back(makeToken(RBRACE, "}", 7));
    tokens.push_back(makeToken(RBRACE, "}", 8));
    tokens.push_back(makeToken(RBRACE, "}", 9));

    Parser parser(tokens);
    HttpConfig http = parser.parseConfig();

    assertEqualInt(1, static_cast<int>(http.servers[0].locations.size()), "server should contain 1 location");
    assertEqualString("/", http.servers[0].locations[0].prefix, "wrong location prefix");
    assertEqualString("./html/site1", http.servers[0].locations[0].root, "wrong location root");
    assertEqualString("home.html", http.servers[0].locations[0].index, "wrong location index");

    printTestOK("parser accepts location /");
}

static void parserAcceptsMethodsGetPost()
{
    printTestHeader("parser accepts methods GET POST");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));
    tokens.push_back(makeToken(WORD, "server", 2));
    tokens.push_back(makeToken(LBRACE, "{", 2));
    tokens.push_back(makeToken(WORD, "listen", 3));
    tokens.push_back(makeToken(WORD, "8080", 3));
    tokens.push_back(makeToken(SEMICOLON, ";", 3));
    tokens.push_back(makeToken(WORD, "location", 4));
    tokens.push_back(makeToken(WORD, "/uploads", 4));
    tokens.push_back(makeToken(LBRACE, "{", 4));
    tokens.push_back(makeToken(WORD, "methods", 5));
    tokens.push_back(makeToken(WORD, "GET", 5));
    tokens.push_back(makeToken(WORD, "POST", 5));
    tokens.push_back(makeToken(SEMICOLON, ";", 5));
    tokens.push_back(makeToken(RBRACE, "}", 6));
    tokens.push_back(makeToken(RBRACE, "}", 7));
    tokens.push_back(makeToken(RBRACE, "}", 8));

    Parser parser(tokens);
    HttpConfig http = parser.parseConfig();

    LocationConfig& loc = http.servers[0].locations[0];
    assertEqualInt(2, static_cast<int>(loc.methods.size()), "location should contain 2 methods");
    assertEqualString("GET", loc.methods[0], "first method should be GET");
    assertEqualString("POST", loc.methods[1], "second method should be POST");
    assertTrue(loc.uploadAuthorised, "POST should enable uploadAuthorised");

    printTestOK("parser accepts methods GET POST");
}

static void parserRejectsDuplicateMethods()
{
    printTestHeader("parser rejects duplicate methods");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));
    tokens.push_back(makeToken(WORD, "server", 2));
    tokens.push_back(makeToken(LBRACE, "{", 2));
    tokens.push_back(makeToken(WORD, "listen", 3));
    tokens.push_back(makeToken(WORD, "8080", 3));
    tokens.push_back(makeToken(SEMICOLON, ";", 3));
    tokens.push_back(makeToken(WORD, "location", 4));
    tokens.push_back(makeToken(WORD, "/", 4));
    tokens.push_back(makeToken(LBRACE, "{", 4));
    tokens.push_back(makeToken(WORD, "methods", 5));
    tokens.push_back(makeToken(WORD, "GET", 5));
    tokens.push_back(makeToken(WORD, "GET", 5));
    tokens.push_back(makeToken(SEMICOLON, ";", 5));
    tokens.push_back(makeToken(RBRACE, "}", 6));
    tokens.push_back(makeToken(RBRACE, "}", 7));
    tokens.push_back(makeToken(RBRACE, "}", 8));

    Parser parser(tokens);

    bool exceptionCatched = false;
    try
    {
        parser.parseConfig();
    }
    catch (const std::exception&)
    {
        exceptionCatched = true;
    }

    assertTrue(exceptionCatched, "parser should reject duplicate methods");
    printTestOK("parser rejects duplicate methods");
}

static void parserRejectsInvalidMethod()
{
    printTestHeader("parser rejects invalid method");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));
    tokens.push_back(makeToken(WORD, "server", 2));
    tokens.push_back(makeToken(LBRACE, "{", 2));
    tokens.push_back(makeToken(WORD, "listen", 3));
    tokens.push_back(makeToken(WORD, "8080", 3));
    tokens.push_back(makeToken(SEMICOLON, ";", 3));
    tokens.push_back(makeToken(WORD, "location", 4));
    tokens.push_back(makeToken(WORD, "/", 4));
    tokens.push_back(makeToken(LBRACE, "{", 4));
    tokens.push_back(makeToken(WORD, "methods", 5));
    tokens.push_back(makeToken(WORD, "PUT", 5));
    tokens.push_back(makeToken(SEMICOLON, ";", 5));
    tokens.push_back(makeToken(RBRACE, "}", 6));
    tokens.push_back(makeToken(RBRACE, "}", 7));
    tokens.push_back(makeToken(RBRACE, "}", 8));

    Parser parser(tokens);

    bool exceptionCatched = false;
    try
    {
        parser.parseConfig();
    }
    catch (const std::exception&)
    {
        exceptionCatched = true;
    }

    assertTrue(exceptionCatched, "parser should reject invalid method");
    printTestOK("parser rejects invalid method");
}

static void parserAcceptsReturn301()
{
    printTestHeader("parser accepts return 301 /new-page");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));
    tokens.push_back(makeToken(WORD, "server", 2));
    tokens.push_back(makeToken(LBRACE, "{", 2));
    tokens.push_back(makeToken(WORD, "listen", 3));
    tokens.push_back(makeToken(WORD, "8080", 3));
    tokens.push_back(makeToken(SEMICOLON, ";", 3));
    tokens.push_back(makeToken(WORD, "location", 4));
    tokens.push_back(makeToken(WORD, "/old-page", 4));
    tokens.push_back(makeToken(LBRACE, "{", 4));
    tokens.push_back(makeToken(WORD, "return", 5));
    tokens.push_back(makeToken(WORD, "301", 5));
    tokens.push_back(makeToken(WORD, "/new-page", 5));
    tokens.push_back(makeToken(SEMICOLON, ";", 5));
    tokens.push_back(makeToken(RBRACE, "}", 6));
    tokens.push_back(makeToken(RBRACE, "}", 7));
    tokens.push_back(makeToken(RBRACE, "}", 8));

    Parser parser(tokens);
    HttpConfig http = parser.parseConfig();

    LocationConfig& loc = http.servers[0].locations[0];
    assertTrue(loc.hasRedirection, "location should have redirection");
    assertEqualInt(301, loc.redirectCode, "wrong redirect code");
    assertEqualString("/new-page", loc.redirectTarget, "wrong redirect target");

    printTestOK("parser accepts return 301 /new-page");
}

static void parserAcceptsHttpErrorPage404()
{
    printTestHeader("parser accepts http error_page 404");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));
    tokens.push_back(makeToken(WORD, "error_page", 2));
    tokens.push_back(makeToken(WORD, "404", 2));
    tokens.push_back(makeToken(WORD, "/errors/404.html", 2));
    tokens.push_back(makeToken(SEMICOLON, ";", 2));
    tokens.push_back(makeToken(WORD, "server", 3));
    tokens.push_back(makeToken(LBRACE, "{", 3));
    tokens.push_back(makeToken(WORD, "listen", 4));
    tokens.push_back(makeToken(WORD, "8080", 4));
    tokens.push_back(makeToken(SEMICOLON, ";", 4));
    tokens.push_back(makeToken(RBRACE, "}", 5));
    tokens.push_back(makeToken(RBRACE, "}", 6));

    Parser parser(tokens);
    HttpConfig http = parser.parseConfig();

    assertTrue(http.errors.find(404) != http.errors.end(), "http should contain error_page 404");
    assertEqualString("/errors/404.html", http.errors[404], "wrong error_page path");

    printTestOK("parser accepts http error_page 404");
}

static void parserAcceptsServerErrorPage404()
{
    printTestHeader("parser accepts server error_page 404");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));
    tokens.push_back(makeToken(WORD, "server", 2));
    tokens.push_back(makeToken(LBRACE, "{", 2));
    tokens.push_back(makeToken(WORD, "listen", 3));
    tokens.push_back(makeToken(WORD, "8080", 3));
    tokens.push_back(makeToken(SEMICOLON, ";", 3));
    tokens.push_back(makeToken(WORD, "error_page", 4));
    tokens.push_back(makeToken(WORD, "404", 4));
    tokens.push_back(makeToken(WORD, "/errors/404.html", 4));
    tokens.push_back(makeToken(SEMICOLON, ";", 4));
    tokens.push_back(makeToken(RBRACE, "}", 5));
    tokens.push_back(makeToken(RBRACE, "}", 6));

    Parser parser(tokens);
    HttpConfig http = parser.parseConfig();

    assertTrue(http.servers[0].errors.find(404) != http.servers[0].errors.end(), "server should contain error_page 404");
    assertEqualString("/errors/404.html", http.servers[0].errors[404], "wrong server error_page path");

    printTestOK("parser accepts server error_page 404");
}

static void parserAppliesInheritanceHttpToServerToLocation()
{
    printTestHeader("parser applies inheritance http to server to location");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));

    tokens.push_back(makeToken(WORD, "keepalive_timeout", 2));
    tokens.push_back(makeToken(WORD, "15", 2));
    tokens.push_back(makeToken(SEMICOLON, ";", 2));

    tokens.push_back(makeToken(WORD, "client_max_body_size", 3));
    tokens.push_back(makeToken(WORD, "1048576", 3));
    tokens.push_back(makeToken(SEMICOLON, ";", 3));

    tokens.push_back(makeToken(WORD, "server", 4));
    tokens.push_back(makeToken(LBRACE, "{", 4));

    tokens.push_back(makeToken(WORD, "listen", 5));
    tokens.push_back(makeToken(WORD, "8080", 5));
    tokens.push_back(makeToken(SEMICOLON, ";", 5));

    tokens.push_back(makeToken(WORD, "location", 6));
    tokens.push_back(makeToken(WORD, "/", 6));
    tokens.push_back(makeToken(LBRACE, "{", 6));
    tokens.push_back(makeToken(WORD, "root", 7));
    tokens.push_back(makeToken(WORD, "./html/site1", 7));
    tokens.push_back(makeToken(SEMICOLON, ";", 7));
    tokens.push_back(makeToken(RBRACE, "}", 8));

    tokens.push_back(makeToken(RBRACE, "}", 9));
    tokens.push_back(makeToken(RBRACE, "}", 10));

    Parser parser(tokens);
    HttpConfig http = parser.parseConfig();

    ServerConfig& srv = http.servers[0];
    LocationConfig& loc = srv.locations[0];

    assertTrue(http.hasKeepalive, "http should have keepalive");
    assertEqualInt(15, http.keepaliveTimeoutSec, "wrong http keepalive");

    assertTrue(srv.hasKeepalive, "server should inherit keepalive");
    assertEqualInt(15, srv.keepaliveTimeoutSec, "wrong server keepalive");

    assertTrue(!loc.hasCgiTimeout, "non-cgi location should not have cgi timeout");

    assertTrue(http.hasMaxBodySize, "http should have max body size");
    assertTrue(http.maxBodySize == 1048576u, "wrong http max body size");

    assertTrue(srv.hasMaxBodySize, "server should inherit max body size");
    assertTrue(srv.maxBodySize == 1048576u, "wrong server max body size");

    assertTrue(loc.hasMaxBodySize, "location should inherit max body size");
    assertTrue(loc.maxBodySize == 1048576u, "wrong location max body size");

    printTestOK("parser applies inheritance http to server to location");
}

static void parserAppliesInheritanceHttpToServerToCgiLocation()
{
    printTestHeader("parser applies inheritance http to server to cgi location");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));

    tokens.push_back(makeToken(WORD, "keepalive_timeout", 2));
    tokens.push_back(makeToken(WORD, "15", 2));
    tokens.push_back(makeToken(SEMICOLON, ";", 2));

    tokens.push_back(makeToken(WORD, "client_max_body_size", 3));
    tokens.push_back(makeToken(WORD, "1048576", 3));
    tokens.push_back(makeToken(SEMICOLON, ";", 3));

    tokens.push_back(makeToken(WORD, "server", 4));
    tokens.push_back(makeToken(LBRACE, "{", 4));

    tokens.push_back(makeToken(WORD, "listen", 5));
    tokens.push_back(makeToken(WORD, "8080", 5));
    tokens.push_back(makeToken(SEMICOLON, ";", 5));

    tokens.push_back(makeToken(WORD, "location", 6));
    tokens.push_back(makeToken(WORD, "/cgi-bin", 6));
    tokens.push_back(makeToken(LBRACE, "{", 6));

    tokens.push_back(makeToken(WORD, "cgi_ext", 7));
    tokens.push_back(makeToken(WORD, ".py", 7));
    tokens.push_back(makeToken(SEMICOLON, ";", 7));

    tokens.push_back(makeToken(WORD, "cgi_path", 8));
    tokens.push_back(makeToken(WORD, "/usr/bin/python3", 8));
    tokens.push_back(makeToken(SEMICOLON, ";", 8));

    tokens.push_back(makeToken(RBRACE, "}", 9));
    tokens.push_back(makeToken(RBRACE, "}", 10));
    tokens.push_back(makeToken(RBRACE, "}", 11));

    Parser parser(tokens);
    HttpConfig http = parser.parseConfig();

    ServerConfig& srv = http.servers[0];
    LocationConfig& loc = srv.locations[0];

    assertTrue(http.hasKeepalive, "http should have keepalive");
    assertEqualInt(15, http.keepaliveTimeoutSec, "wrong http keepalive");

    assertTrue(srv.hasKeepalive, "server should inherit keepalive");
    assertEqualInt(15, srv.keepaliveTimeoutSec, "wrong server keepalive");

    assertTrue(loc.hasCgiTimeout, "cgi location should have an effective cgi timeout");
    assertEqualInt(10, loc.cgiTimeoutSec, "wrong location cgi timeout");

    assertTrue(http.hasMaxBodySize, "http should have max body size");
    assertTrue(http.maxBodySize == 1048576u, "wrong http max body size");

    assertTrue(srv.hasMaxBodySize, "server should inherit max body size");
    assertTrue(srv.maxBodySize == 1048576u, "wrong server max body size");

    assertTrue(loc.hasMaxBodySize, "location should inherit max body size");
    assertTrue(loc.maxBodySize == 1048576u, "wrong location max body size");

    printTestOK("parser applies inheritance http to server to cgi location");
}

static void parserRejectsUnknownDirectiveInServer()
{
    printTestHeader("parser rejects unknown directive in server");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));
    tokens.push_back(makeToken(WORD, "server", 2));
    tokens.push_back(makeToken(LBRACE, "{", 2));
    tokens.push_back(makeToken(WORD, "listen", 3));
    tokens.push_back(makeToken(WORD, "8080", 3));
    tokens.push_back(makeToken(SEMICOLON, ";", 3));
    tokens.push_back(makeToken(WORD, "kapouet", 4));
    tokens.push_back(makeToken(WORD, "42", 4));
    tokens.push_back(makeToken(SEMICOLON, ";", 4));
    tokens.push_back(makeToken(RBRACE, "}", 5));
    tokens.push_back(makeToken(RBRACE, "}", 6));

    Parser parser(tokens);

    bool exceptionCatched = false;
    try
    {
        parser.parseConfig();
    }
    catch (const std::exception&)
    {
        exceptionCatched = true;
    }

    assertTrue(exceptionCatched, "parser should reject unknown directive in server");
    printTestOK("parser rejects unknown directive in server");
}

static void parserRejectsUnknownDirectiveInLocation()
{
    printTestHeader("parser rejects unknown directive in location");

    std::vector<Token> tokens;
    tokens.push_back(makeToken(WORD, "http", 1));
    tokens.push_back(makeToken(LBRACE, "{", 1));
    tokens.push_back(makeToken(WORD, "server", 2));
    tokens.push_back(makeToken(LBRACE, "{", 2));
    tokens.push_back(makeToken(WORD, "listen", 3));
    tokens.push_back(makeToken(WORD, "8080", 3));
    tokens.push_back(makeToken(SEMICOLON, ";", 3));
    tokens.push_back(makeToken(WORD, "location", 4));
    tokens.push_back(makeToken(WORD, "/", 4));
    tokens.push_back(makeToken(LBRACE, "{", 4));
    tokens.push_back(makeToken(WORD, "kapouet", 5));
    tokens.push_back(makeToken(WORD, "42", 5));
    tokens.push_back(makeToken(SEMICOLON, ";", 5));
    tokens.push_back(makeToken(RBRACE, "}", 6));
    tokens.push_back(makeToken(RBRACE, "}", 7));
    tokens.push_back(makeToken(RBRACE, "}", 8));
	Parser parser(tokens);
	bool exceptionCatched = false;
	try
	{
		parser.parseConfig();
	}
	catch (const std::exception&)
	{
		exceptionCatched = true;
	}
	assertTrue(exceptionCatched, "parser should reject unknown directive in location");
	printTestOK("parser rejects unknown directive in location");
}

void	runParserTests()
{
	parserAcceptsMinimalHttpServer();
	parserRejectsEmptyTokenList();
	parserRejectsUnknownTopLevelDirective();
	parserRejectsServerAtTopLevel();
	parserRejectsHttpWithoutOpeningBrace();
	parserRejectsServerWithoutListen();
	parserRejectsInvalidListenPort();
	parserAcceptsHttpKeepaliveTimeout();
	parserAcceptsHttpClientMaxBodySize();
	parserAcceptsServerName();
	parserAcceptsLocationRoot();
	parserAcceptsMethodsGetPost();
	parserRejectsDuplicateMethods();
	parserRejectsInvalidMethod();
	parserAcceptsReturn301();
	parserAcceptsHttpErrorPage404();
	parserAcceptsServerErrorPage404();
	parserAppliesInheritanceHttpToServerToLocation();
    parserAppliesInheritanceHttpToServerToCgiLocation();
	parserRejectsUnknownDirectiveInServer();
	parserRejectsUnknownDirectiveInLocation();
}
