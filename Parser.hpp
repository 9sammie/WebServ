/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 11:36:55 by vakozhev          #+#    #+#             */
/*   Updated: 2026/02/24 13:06:19 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP
#include <vector>
#include <cstddef>
#include "Token.hpp"
#include "Config.hpp"

struct displayState
{
	const std::vector<Token>& toks;
	size_t pos;
	displayState(const std::vector<Token>& t) : toks(t), pos(0) {}
};

#endif
