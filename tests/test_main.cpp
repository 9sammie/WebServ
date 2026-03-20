/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_main.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 14:06:31 by vakozhev          #+#    #+#             */
/*   Updated: 2026/03/16 14:41:03 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

void runLexerTests();

int main(void)
{
	try
	{
		runLexerTests();
		std::cout << "All the lexer tests have passed." << std::endl;
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Test failed: " << e.what() << std::endl;
		return 1;
	}
}
