/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testsHelper.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 12:31:45 by vakozhev          #+#    #+#             */
/*   Updated: 2026/03/16 14:35:10 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "testsHelper.hpp"
#include <stdexcept>
#include <iostream>

void assertTrue(bool condition, const std::string& message)
{
	if (!condition)
		throw std::runtime_error(message);
}

void assertEqualInt(int expected, int actual, const std::string& message)
{
	if (expected != actual)
		throw std::runtime_error(message);
}

void assertEqualString(const std::string& expected, const std::string& actual, const std::string& message)
{
	if (expected != actual)
		throw std::runtime_error(message);
}

void printTestHeader(const std::string& name)
{
	std::cout<< "[TEST]" << name <<std::endl;
}

void printTestOK(const std::string& name)
{
	std::cout << "[OK]" << name <<std::endl;
}  
