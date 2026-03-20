#include "ServerManager.hpp"
#include <iostream>
#include <list>
#include <exception>
#include "Signal.hpp"
#include "Config.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

int main(int ac, char** av){
    if (ac != 2){
        if (ac < 2)
            std::cerr << "Error: Webserv need a config file.";
        else if (ac > 2)
            std::cerr << "Error: Webserv only need a config file.";
        return 1;
    }

    // signal(SIGPIPE, SIG_IGN); // For MacOS only, and change sendResponse function too, uncomment
    init_signal_handler();

    try {
        Lexer lexer;
        std::vector<Token> tokens = lexer.lexFile(av[1]);
        
        Parser parser(tokens);
        HttpConfig httpConfig = parser.parseConfig();

        ServerManager manager(httpConfig);
        manager.run();
    }
    catch (std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return (1);
    }

    return (0);
}