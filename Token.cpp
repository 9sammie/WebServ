/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 15:02:33 by vakozhev          #+#    #+#             */
/*   Updated: 2026/02/19 16:04:23 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Token.hpp"

Token::Token() : type(WORD), wordText(""), line(1) // je cree un token vide p ex Token t;
{}

Token::Token(TokenType t, const std::string& str, int l) // liste d initialisation
	: type(t), wordText(str), line(l) //je cree un token rempli, p ex Token t(WORD, "server", 12)
{}
