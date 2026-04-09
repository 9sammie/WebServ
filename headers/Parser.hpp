/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 11:36:55 by vakozhev          #+#    #+#             */
/*   Updated: 2026/03/24 19:03:38 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include <cstddef>
#include <map>

#include "Token.hpp"
#include "Config.hpp"

class Parser
{
	public:
		Parser(const std::vector<Token>& toks);
		~Parser();

		HttpConfig parseConfig();

	private:
		Parser(const Parser& src);
		Parser& operator=(const Parser& src);

		const std::vector<Token>& _toks;
		std::size_t _pos;

		/* token navigation */
		bool isNotEnd() const;
		bool checkType(TokenType expectedType) const;
		bool checkWord(const std::string& w) const;
		const Token& consume(TokenType expectedType);
		const Token& consumeWord(const std::string& w);
		const Token& currentToken() const;
		const Token& previousToken() const;

		/* value parsing */
		std::vector<std::string> readDirectiveArgs(const Token& directiveTok);
		bool parseOnOffArg(const Token& directiveTok, const std::vector<std::string>& args);
		int parsePositiveInt(const Token& directiveTok, const std::string& s) const;
		std::size_t parseSizeT(const Token& directiveTok, const std::string& s) const;
		int parsePort(const Token& directiveTok, const std::string& s);
		ListenConfig parseListenArg(const Token& directiveTok, const std::string& s);

		/* directive parsing */
		void parseErrorPage(std::map<int, std::string>& errors, const Token& directiveTok, const std::vector<std::string>& args);
		void applyMethods(LocationConfig& loc, const Token& directiveTok, const std::vector<std::string>& args);
		void parseHttpDirective(HttpConfig& http, const Token& nameTok);
		void parseServerDirective(ServerConfig& srv, const Token& nameTok);
		void parseLocationDirective(LocationConfig& loc, const Token& nameTok);

		/* parsing of blocks */
		HttpConfig parseHttpBlock(const Token& httpTok);
		ServerConfig parseServerBlock(const Token& serverTok);
		LocationConfig parseLocationBlock(const Token& locationTok);

		/* effective data for timeout && body_size */
		void applyEffectiveData(HttpConfig& http);

		/* error handling */	
		std::string formatError(const Token& tok, const std::string& msg) const;
		void throwUnexpectedEof(const std::string& expected) const;
		void throwUnexpectedToken(const Token& tok) const;
		void throwInvalidArgs(const Token& directiveTok) const;
		void throwUnknownDirective(const Token& directiveTok) const;
		void throwDirectiveNotAllowedHere(const Token& directiveTok) const;
		void throwDirectiveNotTerminated(const Token& directiveTok) const;
		void throwNoOpeningBrace(const Token& directiveTok) const;
		void throwInvalidValue(const Token& directiveTok, const std::string& value) const;
		void throwDuplicateValue(const Token& directiveTok, const std::string& value) const;		
};

#endif
