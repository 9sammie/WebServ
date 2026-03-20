#include "ServerManager.hpp"
#include <iostream>
#include <list>
#include <exception>
#include "Signal.hpp"
#include "Config.hpp"

int main(void){

    // signal(SIGPIPE, SIG_IGN); // For MacOS only, and change sendResponse function too, uncomment
    init_signal_handler();
    HttpConfig httpConfig;
    // std::list<int> ports;// Hardcoded, will use all ports listeners found inside serverConfig
    // ports.push_back(8080); // Hardcoded will need HttpConfig
    try {
        ServerManager manager(httpConfig);
        manager.run();
    }
    catch (std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return (1);
    }

    return (0);
}