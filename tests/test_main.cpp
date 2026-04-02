/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_main.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 14:06:31 by vakozhev          #+#    #+#             */
/*   Updated: 2026/03/25 17:29:08 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

void runLexerTests();
void runParserTests();

int main(void)
{
	try
	{
		runLexerTests();
		std::cout << "All the lexer tests have passed." << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Test failed: " << e.what() << std::endl;
		return 1;
	}
	try
	{
		runParserTests();
		std::cout << "All the parser tests have passed." << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Test failed: " << e.what() << std::endl;
		return 1;
	}
	std::cout << "All tests have passed." << std::endl;
	return 0;
}
