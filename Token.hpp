/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 14:50:54 by vakozhev          #+#    #+#             */
/*   Updated: 2026/02/18 16:25:14 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_HPP
#define TOKEN_HPP
#include <string>

class Token
{
	public:
		enum ElementType
		{
			WORD,
			LBRACE,
			RBRACE,
			SEMICOLON,
			COMMENT
		};
		
		ElementType type;
		std::string wordText;
		int line;

		Token();
		Token(ElementType t, const std::string& str, int line);
		Token(const Token& other);
		~Token();
		Token& operator=(const Token& other);
};

#endif
		
		



















































//
    //struct Token {
        //TokenType type;
        //std::string text; // seulement pour TOK_WORD
        //int line;         // ligne où le token a commencé
//
        //Token();
        //Token(TokenType t, const std::string& s, int l);
    //};
//
    //// --- 
    //Tokenizer();
    //Tokenizer(const Tokenizer& other);
    //Tokenizer& operator=(const Tokenizer& other);
    //~Tokenizer();
//
    //// --- 
    //std::vector<Token> tokenizeFile(const std::string& path);
//
//private:
    //// --- Helpers  ---
    //void resetState_();
    //void flushWord_(std::vector<Token>& out);
    //void pushDelimiter_(std::vector<Token>& out, char ch);
    //void skipComment_(std::istream& in);
//
//private:
    //// --- Etat  ---
    //std::string _buf;
    //int _line;
//};
//
//#endif
