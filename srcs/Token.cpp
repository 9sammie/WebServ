#include "Token.hpp"

Token::Token() : type(WORD), wordText(""), line(1)
{}

Token::Token(TokenType t, const std::string& str, int l)
	: type(t), wordText(str), line(l)
{}
