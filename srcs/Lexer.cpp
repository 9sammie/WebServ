#include "Lexer.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <sstream>

Lexer::Lexer() : _buf("") {}

Lexer::Lexer(const Lexer& src) : _buf(src._buf) {}

Lexer::~Lexer() {}

Lexer& Lexer::operator=(const Lexer& src)
{
	if (this != &src)
		this->_buf = src._buf;
	return *this;
}

std::vector<Token> Lexer::lexFile(const std::string& path)
{
	std::vector<std::string> stack; 
	std::vector<Token> raw = lexFileRaw(path);
	return expandIncludes(raw, path, stack, 0);
}

/* helpers ********************************************************/

std::string Lexer::resolveIncludedFile(const std::vector<Token>& in, size_t i, const std::string& currentFile) const
{
	const std::string& rawPath = in[i + 1].wordText;
	return resolveIncludePath(rawPath, currentFile);
} 

std::string Lexer::dirnameOf(const std::string& path)
{
	std::string::size_type pos = path.rfind('/');
	if (pos == std::string::npos)
		return ".";
	if (pos == 0)
		return "/";
	return path.substr(0, pos);
}

bool Lexer::isWhitespace(char c)
{
	if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
		return true;
	return false;
}

bool Lexer::isDelim(char c)
{
	if (c == '{' || c == '}' || c == ';')
		return true;
	return false;
}

bool Lexer::isBlankLine(const std::string& str)
{
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (!isWhitespace(str[i]))
			return false;
	}
	return true;
}

void Lexer::skipComment(std::string& str)
{
	size_t pos = str.find('#');
	if (pos != std::string::npos)
		str.erase(pos);
}

/* lexer funcs + include processing ******************************/

std::vector<Token> Lexer::lexFileRaw(const std::string& path)
{
    std::ifstream infile(path.c_str());
    if (!infile.is_open())
        throw std::runtime_error(formatError(path, 0, "open() \"" + path + "\" failed"));
    std::vector<Token> res;
    int line = 1;
    while (std::getline(infile, _buf))
    {
        skipComment(_buf);
        if (isBlankLine(_buf)) {
            ++line;
            continue;
        }

        lexLine(_buf, line, res, path);
        ++line;
    }
	
    if (res.empty())
        throw std::runtime_error(formatError(path, 0, "no tokens found in configuration"));

    return res;
}

void Lexer::validateIncludeDirective(const std::vector<Token>& in,
                                     size_t i,
                                     const std::string& currentFile) const
{
    int line = 0;
    if (i < in.size())
        line = in[i].line;

    if (i + 2 >= in.size())
        throwIncludeError(currentFile, line,
            "invalid number of arguments in \"include\" directive");

    if (in[i + 1].type != WORD)
        throwIncludeError(currentFile, line,
            "invalid value in \"include\" directive");

    if (in[i + 2].type != SEMICOLON)
        throwIncludeError(currentFile, line,
            "directive \"include\" is not terminated by \";\"");
}

std::string Lexer::resolveIncludePath(const std::string& inc, const std::string& currentFile)
{
	if (!inc.empty() && inc[0] == '/')
		return inc;
	std::string dir = dirnameOf(currentFile);
	return dir + "/" + inc;
}

std::vector<Token> Lexer::expandIncludes(const std::vector<Token>& in,
                                        const std::string& currentFile,
                                        std::vector<std::string>& includeStack,
                                        int depth)
{
	if (depth > 5)
		throw std::runtime_error(formatError(currentFile, 0, "too many nested includes"));
	if (std::find(includeStack.begin(), includeStack.end(), currentFile) != includeStack.end())
		throw std::runtime_error(formatError(currentFile, 0, "include cycle detected"));
    includeStack.push_back(currentFile);
    std::vector<Token> out;
    out.reserve(in.size());

    for (size_t i = 0; i < in.size(); )
    {
		if (isIncludeDirective(in, i))
		{
			validateIncludeDirective(in, i, currentFile);
			std::string incPath = resolveIncludedFile(in, i, currentFile);
			std::vector<Token> incRaw = lexFileRaw(incPath);
			std::vector<Token> incExpanded = expandIncludes(incRaw, incPath, includeStack, depth + 1);
			out.insert(out.end(), incExpanded.begin(), incExpanded.end());
			i = i + 3;
			continue;
		}
		out.push_back(in[i]);
		++i;
	}
	includeStack.pop_back();
	return out;
}

void Lexer::lexLine(const std::string& str, int line, std::vector<Token>& res, const std::string& path)
{
    std::size_t i = 0;
    while (i < str.size())
    {
        while (i < str.size() && isWhitespace(str[i]))
            ++i;

        if (i >= str.size())
            return;

        if (str[i] == '{')
        {
            res.push_back(Token(LBRACE, "{", line));
            ++i;
            continue;
        }

        if (str[i] == '}')
        {
            res.push_back(Token(RBRACE, "}", line));
            ++i;
            continue;
        }

        if (str[i] == ';')
        {
            res.push_back(Token(SEMICOLON, ";", line));
            ++i;
            continue;
        }

        std::size_t start = i;
        while (i < str.size() && !isWhitespace(str[i]) && !isDelim(str[i]) && str[i] != '#')
            ++i;

        if (i > start)
        {
            res.push_back(Token(WORD, str.substr(start, i - start), line));
            continue;
        }

        throwLexError(path, line,
            std::string("unexpected character \"") + str[i] + "\"");
    }
}

bool Lexer::isIncludeDirective(const std::vector<Token>& in, size_t i) const
{
	if (i < in.size() && in[i].type == WORD && in[i].wordText == "include")
		return true;
	else
		return false;
}

/* errors **********************************************************/

std::string Lexer::formatError(const std::string& file, int line, const std::string& msg) const
{
    std::ostringstream out;
    out << "webserv: [emerg] " << msg;
    if (!file.empty())
        out << " in " << file;
    if (line > 0)
        out << ":" << line;
    return out.str();
}

void Lexer::throwLexError(const std::string& file, int line, const std::string& msg) const
{
    throw std::runtime_error(formatError(file, line, msg));
}

void Lexer::throwIncludeError(const std::string& file, int line, const std::string& msg) const
{
    throw std::runtime_error(formatError(file, line, msg));
}