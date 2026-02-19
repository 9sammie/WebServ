/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 10:43:41 by vakozhev          #+#    #+#             */
/*   Updated: 2026/02/19 11:00:30 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
#define LEXER_HPP

#include <vector>
#include <string>
#include "Token.hpp"

class Lexer
	public:
		Lexer();
		Lexer(const Lexer& other);
		~Lexer();
		Lexer& operator=(const Lexer& other);
		std::vector<Token> ??
	private:
		std::string _buf;
