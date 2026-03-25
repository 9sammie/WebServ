/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testsHelper.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 12:29:59 by vakozhev          #+#    #+#             */
/*   Updated: 2026/03/16 12:40:58 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef TESTSHELPER_HPP
#define TESTSHELPER_HPP

#include <string>

void assertTrue(bool condition, const std::string& message);
void assertEqualInt(int expected, int actual, const std::string& message);
void assertEqualString(const std::string& expected, const std::string& actual, const std::string& message);
void printTestHeader(const std::string& name);
void printTestOK(const std::string& name);

#endif
