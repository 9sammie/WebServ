/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 11:36:55 by vakozhev          #+#    #+#             */
/*   Updated: 2026/03/13 13:57:05 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP
#include <vector>
#include <string>
#include <cstddef>
#include "Token.hpp"
#include "Config.hpp"

class Parser
{
	public:
		Parser(const std::vector<Token>& toks);
		HttpConfig parseConfig();
		HttpConfig parseHttpBlock();
		ServerConfig parseServerBlock();
		LocationConfig parseLocationBlock();		
		//LocationConfig parseFirstLocationInFile(); // a voir si garder comme cela
	private:
		const std::vector<Token>& _toks;
		std::size_t _pos;
		bool isNotEnd() const;
		bool checkType(TokenType expectedType) const;
		const Token& consume(TokenType expectedType);
		bool checkWord(const std::string& w) const;
		void consumeWord(const std::string& w);
		void skipBlock();
		std::size_t parseSizeT(const std::string& s, const std::string& name);
		int parsePositiveInt(const std::string& s, const std::string& name);
		std::vector<std::string> readDirectiveArgs();
		ListenConfig parseListenArg(const std::string& s);
		int parsePort(const std::string& s);
		void parseErrorPage(std::map<int, std::string>& errors, const std::vector<std::string>& args, const std::string& name);
		bool parseOnOffArg(const std::vector<std::string>& args);
		void parseLocationDirective(LocationConfig& loc, const std::string& name);
		void parseServerDirective(ServerConfig& srv, const std::string& name);
		void parseHttpDirective(HttpConfig& http, const std::string& name);
		void applyMethods(LocationConfig& loc, const std::vector<std::string>& args);
		void applyEffectifData(HttpConfig& http);
		//ListenConfig Parser::parseListenArg(const std::string& s);
		
};

#endif
