/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 10:43:41 by vakozhev          #+#    #+#             */
/*   Updated: 2026/03/04 16:53:14 by vakozhev         ###   ########lyon.fr   */
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
		Lexer(const Lexer& src);
		~Lexer();
		Lexer& operator=(const Lexer& src);
		std::vector<Token> lexFile(const std::string& path); //path = nom de fichier de config
	private:
		std::string _buf;
		void lexLine(const std::string& str, int line, std::vector<Token>& res);
		static bool isWhitespace(char c);
		static bool isDelim(char c);
		static bool isBlankLine(const std::string& str);
		static void skipComment(std::string& str);

		//void lexLine(const std::string& str, int line, std::vector<Token>& res);
		std::vector<Token> lexFileRaw(const std::string& path);
		std::vector<Token> expandIncludes(const std::vector<Token>& in, const std::string& currentFile, std::vector<std::string>& includeStack, int depth);
		static std::string dirnameOf(const std::string& path);
		static std::string resolveIncludePath(const std::string& inc, const std::string& currentFile);
};

#endif
