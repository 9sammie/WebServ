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
