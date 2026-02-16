#include "ServerManager.hpp"
#include <iostream>
#include <list>
#include <exception>
// #include <signal.h> //Uncomment for MacOS

int main(void){

    // signal(SIGPIPE, SIG_IGN); // For MacOS only, and change sendResponse function too, uncomment
    
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