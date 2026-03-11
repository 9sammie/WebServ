#include <unistd.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <cstring>

// Forward declaration - same struct as CgiHandler.cpp
typedef struct DataCgi{
    std::string method;
    std::string URI;
    std::string queryString;
    std::map<std::string, std::string> headers;
    std::string body;
    std::string scriptPath;
    std::string interpreter;
}DataCgi;

int CgiHandler(DataCgi data);

int main(){
    // --- Test 1: GET request ---
    std::cout << "=== Test GET ===" << std::endl;
    {
        DataCgi data;
        data.method      = "GET";
        data.URI         = "/cgi-bin/hello.py";
        data.queryString = "name=World";
        data.scriptPath  = "/home/ctheveno/Documents/Projects/milestone_5/WebServ_Rats_port_80/SubnetMaster/hello.py";
        data.interpreter = "/usr/bin/python3";
        data.headers["host"]       = "localhost";
        data.headers["user-agent"] = "TestClient/1.0";

        int pipeFd = CgiHandler(data);
        if (pipeFd < 0){
            std::cerr << "CgiHandler failed" << std::endl;
            return 1;
        }

        // Read CGI output
        char    buf[4096];
        ssize_t n;
        std::string response;
        while ((n = read(pipeFd, buf, sizeof(buf) - 1)) > 0){
            buf[n] = '\0';
            response += buf;
        }
        close(pipeFd);

        std::cout << "--- CGI output ---" << std::endl;
        std::cout << response << std::endl;
    }

    // --- Test 2: POST request ---
    std::cout << "=== Test POST ===" << std::endl;
    {
        DataCgi data;
        data.method      = "POST";
        data.URI         = "/cgi-bin/hello.py";
        data.queryString = "";
        data.scriptPath  = "/home/ctheveno/Documents/Projects/milestone_5/WebServ_Rats_port_80/SubnetMaster/hello.py";
        data.interpreter = "/usr/bin/python3";
        data.body        = "name=PostWorld&lang=C++";
        data.headers["host"]         = "localhost";
        data.headers["content-type"] = "application/x-www-form-urlencoded";

        int pipeFd = CgiHandler(data);
        if (pipeFd < 0){
            std::cerr << "CgiHandler failed" << std::endl;
            return 1;
        }

        char    buf[4096];
        ssize_t n;
        std::string response;
        while ((n = read(pipeFd, buf, sizeof(buf) - 1)) > 0){
            buf[n] = '\0';
            response += buf;
        }
        close(pipeFd);

        std::cout << "--- CGI output ---" << std::endl;
        std::cout << response << std::endl;
    }

    return 0;
}
