/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 14:50:54 by vakozhev          #+#    #+#             */
/*   Updated: 2026/02/19 14:42:57 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_HPP
#define TOKEN_HPP
#include <string>


	enum TokenType
	{
		WORD,
		LBRACE,
		RBRACE,
		SEMICOLON,
		COMMENT
	};
		
	struct Token
	{
		Token();
		Token(TokenType t, const std::string& str, int l);
		TokenType type;
		std::string wordText;
		int line;
	};

#endif
