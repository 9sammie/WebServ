/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 13:00:35 by vakozhev          #+#    #+#             */
/*   Updated: 2026/02/18 15:02:20 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"

Lexer::Token() : type(WORD), wordText(""), line(1)
{}
Lexer::Token(ElementType t, const std::string& str, int l)
	: type(t)
	, wordText(str), line(l)
{}

Lexer::
















































//
//#include <fstream>
//#include <stdexcept>
//#include <cctype>   // std::isspace
//#include <istream>
//
//// ----- Token -----
//Tokenizer::Token::Token() : type(TOK_WORD), text(""), line(1) {}
//
//Tokenizer::Token::Token(TokenType t, const std::string& s, int l)
//: type(t), text(s), line(l) {}
//
//
//// ----- Canonical form -----
//Tokenizer::Tokenizer() : _buf(""), _line(1) {}
//
//Tokenizer::Tokenizer(const Tokenizer& other) : _buf(other._buf), _line(other._line) {}
//
//Tokenizer& Tokenizer::operator=(const Tokenizer& other) {
    //if (this != &other) {
        //_buf = other._buf;
        //_line = other._line;
    //}
    //return *this;
//}
//
//Tokenizer::~Tokenizer() {}
//
//
//// ----- Private helpers -----
//void Tokenizer::resetState_() {
    //_buf.clear();
    //_line = 1;
//}
//
//void Tokenizer::flushWord_(std::vector<Token>& out) {
    //if (!_buf.empty()) {
        //out.push_back(Token(TOK_WORD, _buf, _line));
        //_buf.clear();
    //}
//}
//
//void Tokenizer::pushDelimiter_(std::vector<Token>& out, char ch) {
    //TokenType t = TOK_WORD;
    //if (ch == '{') t = TOK_LBRACE;
    //else if (ch == '}') t = TOK_RBRACE;
    //else if (ch == ';') t = TOK_SEMI;
    //out.push_back(Token(t, "", _line));
//}
//
//void Tokenizer::skipComment_(std::istream& in) {
    //// On est juste après avoir lu '#'
    //char c;
    //while (in.get(c)) {
        //if (c == '\n') {
            //_line++;
            //break;
        //}
    //}
//}
//
//
//// ----- Public API -----
//std::vector<Tokenizer::Token> Tokenizer::tokenizeFile(const std::string& path) {
    //std::ifstream in(path.c_str());
    //if (!in.is_open())
        //throw std::runtime_error("Tokenizer: cannot open file: " + path);
//
    //resetState_();
//
    //std::vector<Token> out;
    //char ch;
//
    //while (in.get(ch)) {
//
        //// Newline
        //if (ch == '\n') {
            //flushWord_(out);
            //_line++;
            //continue;
        //}
//
        //// Comment
        //if (ch == '#') {
            //flushWord_(out);
            //skipComment_(in);
            //continue;
        //}
//
        //// Whitespace (space/tab/etc.)
        //if (std::isspace(static_cast<unsigned char>(ch))) {
            //flushWord_(out);
            //continue;
        //}
//
        //// Delimiters
        //if (ch == '{' || ch == '}' || ch == ';') {
            //flushWord_(out);
            //pushDelimiter_(out, ch);
            //continue;
        //}
//
        //// WORD char
        //_buf += ch;
    //}
//
    //// EOF: flush last word
    //flushWord_(out);
    //return out;
//}
