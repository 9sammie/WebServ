#ifndef LEXER_HPP
#define LEXER_HPP

#include <vector>
#include <string>

#include "Token.hpp"

class Lexer
{
	public:
		Lexer();
		Lexer(const Lexer& src);
		~Lexer();
		Lexer& operator=(const Lexer& src);
		std::vector<Token> lexFile(const std::string& path);
	private:
		std::string _buf;

		/* helpers */
		static bool isWhitespace(char c);
		static bool isDelim(char c);
		static bool isBlankLine(const std::string& str);
		static void skipComment(std::string& str);
		static std::string dirnameOf(const std::string& path);
		std::string resolveIncludedFile(const std::vector<Token>& in, size_t i, const std::string& currentFile) const;

		/* lexer funcs + include processing */
		std::vector<Token> lexFileRaw(const std::string& path);
		std::vector<Token> expandIncludes(const std::vector<Token>& in, const std::string& currentFile, std::vector<std::string>& includeStack, int depth);
		bool isIncludeDirective(const std::vector<Token>& in, size_t i) const;
		void validateIncludeDirective(const std::vector<Token>& in, size_t i, const std::string& currentFile) const;
		static std::string resolveIncludePath(const std::string& inc, const std::string& currentFile);
		void lexLine(const std::string& str, int line, std::vector<Token>& res, const std::string& path);
		
		/* errors */
		std::string formatError(const std::string& file, int line, const std::string& msg) const;
		void throwLexError(const std::string& file, int line, const std::string& msg) const;
		void throwIncludeError(const std::string& file, int line, const std::string& msg) const;
};

#endif
