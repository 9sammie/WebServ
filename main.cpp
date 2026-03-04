#include "Lexer.hpp"
#include "Parser.hpp"
#include <iostream>

static void expectWord(displayState& ds, const std::string& w)
{
    if (ds.pos >= ds.toks.size() || ds.toks[ds.pos].type != WORD || ds.toks[ds.pos].wordText != w)
        throw std::runtime_error("expected word: " + w);
    ds.pos++;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <conf>\n";
        return 1;
    }

    try {
        Lexer lx;
        std::vector<Token> toks = lx.lexFile(argv[1]);
        displayState ds(toks);

        // on saute jusqu'à trouver "location"
        while (ds.pos < ds.toks.size()) {
            if (ds.toks[ds.pos].type == WORD && ds.toks[ds.pos].wordText == "location")
                break;
            ds.pos++;
        }
        if (ds.pos >= ds.toks.size())
            throw std::runtime_error("no 'location' found");

        // consomme "location" puis parse le block
        expectWord(ds, "location");
        LocationConfig loc = parseLocationBlock(ds); // ta version sans locationLine

        std::cout << "prefix = " << loc.prefix << "\n";
        std::cout << "root = " << loc.root << "\n";
        std::cout << "index = " << loc.index << "\n";
        std::cout << "autoindex = " << (loc.autoindex ? "on" : "off") << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }
    return 0;
}
