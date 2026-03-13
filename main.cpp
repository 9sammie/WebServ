/*#include "Lexer.hpp"
#include "Parser.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

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
}*/

/*int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <conf>\n";
        return 1;
    }

    try
    {
        Lexer lx;
        std::vector<Token> toks = lx.lexFile(argv[1]);

        Parser p(toks);

        LocationConfig loc = p.parseFirstLocationInFile();

        std::cout << "prefix    = " << loc.prefix << "\n";
        std::cout << "root      = " << loc.root << "\n";
        std::cout << "index     = " << loc.index << "\n";
        std::cout << "autoindex = " << (loc.autoindex ? "on" : "off") << "\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }

    return 0;
}*/

//main vient de gpt juste pour le test//
#include "Lexer.hpp"
#include "Parser.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <string>

static void printMethods(const std::vector<std::string>& methods)
{
    std::cout << "[";
    for (size_t i = 0; i < methods.size(); ++i)
    {
        if (i) std::cout << ", ";
        std::cout << methods[i];
    }
    std::cout << "]";
}

static void printErrors(const std::map<int, std::string>& errors, const std::string& indent)
{
    if (errors.empty())
    {
        std::cout << indent << "(none)\n";
        return;
    }
    for (std::map<int, std::string>::const_iterator it = errors.begin(); it != errors.end(); ++it)
        std::cout << indent << it->first << " -> " << it->second << "\n";
}

static void dumpLocation(const LocationConfig& loc, const std::string& indent)
{
    std::cout << indent << "prefix            = " << loc.prefix << "\n";
    std::cout << indent << "root              = " << loc.root << "\n";
    std::cout << indent << "index             = " << loc.index << "\n";
    std::cout << indent << "autoindex         = " << (loc.autoindex ? "on" : "off") << "\n";

    std::cout << indent << "methods           = ";
    printMethods(loc.methods);
    std::cout << "\n";

    std::cout << indent << "uploadAuthorised  = " << (loc.uploadAuthorised ? "true" : "false") << "\n";

    std::cout << indent << "cgiAuthorised     = " << (loc.cgiAuthorised ? "true" : "false") << "\n";
    std::cout << indent << "cgiExt            = " << loc.cgiExt << "\n";
    std::cout << indent << "cgiPath           = " << loc.cgiPath << "\n";

    std::cout << indent << "hasRedirection    = " << (loc.hasRedirection ? "true" : "false") << "\n";
    if (loc.hasRedirection)
    {
        std::cout << indent << "redirectCode      = " << loc.redirectCode << "\n";
        std::cout << indent << "redirectTarget    = " << loc.redirectTarget << "\n";
    }

    std::cout << indent << "hasMaxBodySize    = " << (loc.hasMaxBodySize ? "true" : "false") << "\n";
    std::cout << indent << "maxBodySize       = " << loc.maxBodySize << "\n";

    std::cout << indent << "hasKeepalive      = " << (loc.hasKeepalive ? "true" : "false") << "\n";
    std::cout << indent << "keepaliveTimeout  = " << loc.keepaliveTimeoutSec << "\n";
}

static void dumpServer(const ServerConfig& srv, size_t idx)
{
    std::cout << "\n[SERVER " << idx << "]\n";

    std::cout << "serverName           = " << srv.serverName << "\n";

    std::cout << "keepaliveTimeoutSec  = " << srv.keepaliveTimeoutSec
              << " (hasKeepalive=" << (srv.hasKeepalive ? "true" : "false") << ")\n";

    std::cout << "maxBodySize          = " << srv.maxBodySize
              << " (hasMaxBodySize=" << (srv.hasMaxBodySize ? "true" : "false") << ")\n";

    std::cout << "listens count        = " << srv.listens.size() << "\n";
    for (size_t i = 0; i < srv.listens.size(); ++i)
    {
        std::cout << "  listen[" << i << "] host=" << srv.listens[i].host
                  << " port=" << srv.listens[i].port << "\n";
    }

    std::cout << "error pages:\n";
    printErrors(srv.errors, "  ");

    std::cout << "locations count      = " << srv.locations.size() << "\n";
    for (size_t i = 0; i < srv.locations.size(); ++i)
    {
        std::cout << "  [LOCATION " << i << "]\n";
        dumpLocation(srv.locations[i], "    ");
        std::cout << "\n";
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <conf>\n";
        return 1;
    }

    try
    {
        Lexer lx;
        std::vector<Token> toks = lx.lexFile(argv[1]);

        Parser p(toks);
        HttpConfig http = p.parseConfig();

        std::cout << "[HTTP]\n";
        std::cout << "keepaliveTimeoutSec = " << http.keepaliveTimeoutSec << "\n";
        std::cout << "maxBodySize         = " << http.maxBodySize << "\n";

        std::cout << "error pages:\n";
        printErrors(http.errors, "  ");

        std::cout << "servers count       = " << http.servers.size() << "\n";

        for (size_t i = 0; i < http.servers.size(); ++i)
            dumpServer(http.servers[i], i);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }

    return 0;
}
