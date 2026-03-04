/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 10:43:41 by vakozhev          #+#    #+#             */
/*   Updated: 2026/03/04 10:38:52 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
#define LEXER_HPP

#include <vector>
#include <string>
#include "Token.hpp"

class Lexer
{
	public:
		Lexer();
		Lexer(const Lexer& other);
		~Lexer();
		Lexer& operator=(const Lexer& other);
		std::vector<Token> lexFile(const std::string& path); //path = nom de fichier de config
	private:
		std::string _buf;
		static bool isWhitespace(char c);
		static bool isDelim(char c);
		static bool isBlankLine(const std::string& str);
		static void skipComment(std::string& str);

		void lexLine(const std::string& str, int line, std::vector<Token>& res);
};
#endif
