#include "ServerManager.hpp"
#include <iostream>
#include <list>
#include <exception>
#include "Signal.hpp"

int main(void){

    // signal(SIGPIPE, SIG_IGN); // For MacOS only, and change sendResponse function too, uncomment
    init_signal_handler();
    std::list<int> ports;
    ports.push_back(8080);

    try {
        ServerManager manager(ports);
        manager.run();
    }
    catch (std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return (1);
    }

    return (0);
}