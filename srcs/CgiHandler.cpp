#include <unistd.h>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include "Client.hpp"

typedef struct DataCgi{
    std::string method; //GET, POST, DELETE
    std::string URI; // /cgi-bin/cgi.py
    std::string queryString; //part after ? in URI, first line of request
    std::map<std::string, std::string> headers;
    std::string body;
    std::string scriptPath; //Absolute path of script
    std::string interpreter; //ex : /usr/bin/python3
    std::string serverPort;
    std::string contentType;
    std::string contentLength;
    std::string serverName;
    std::string pathInfo;

    std::string scriptName;
    std::string remoteAddr; //adress IP client
    std::string remotePort;
    std::string scriptFilename; // equal to scriptPath but used by cgi-PHP
    std::string redirectStatus;// used by php, usually 200, could crash if not present
}DataCgi;

Client::CgiInfo failedCgiHandler(){
    Client::CgiInfo fail;
    fail.isCgi = false;
    fail.pipeRead = -1;
    fail.pipeWrite = -1;
    fail.pid = -1;
    fail.start_time = -1;
    fail.bodyWrittenBytes = 0;
    return fail;
}

std::vector<std::string> buildEnvpData(DataCgi data){
    std::vector<std::string> envpData;

    envpData.push_back("REQUEST_METHOD=" + data.method);
    envpData.push_back("QUERY_STRING=" + data.queryString);
    envpData.push_back("SCRIPT_NAME=" + data.URI);
    envpData.push_back("GATEWAY_INTERFACE=CGI/1.1");
    envpData.push_back("SERVER_PROTOCOL=HTTP/1.1");

    for (std::map<std::string, std::string>::const_iterator it = data.headers.begin(); it != data.headers.end(); ++it){
         std::string key = it->first;
        for(size_t i = 0; i <key.size(); ++i){
            if (key[i] == '-')
                key[i] = '_';
            key[i] = std::toupper((unsigned char)key[i]);
        }
        envpData.push_back("HTTP_" + key + "=" + it->second);
    }
    envpData.push_back("CONTENT_TYPE=" + data.contentType);
    envpData.push_back("CONTENT_LENGTH=" + data.contentLength);
    envpData.push_back("SERVER_PORT=" + data.serverPort);
    envpData.push_back("SERVER_NAME=" + data.serverName);
    envpData.push_back("PATH_INFO=" + data.pathInfo);
    return envpData;
}

Client::CgiInfo CgiHandler(DataCgi data){
    //Initialize pipes
    int     pipeIn[2];
    int     pipeOut[2];
    if (pipe(pipeIn) < 0)
        return failedCgiHandler();
    if (pipe(pipeOut) < 0){
        close(pipeIn[0]);
        close(pipeIn[1]);
        return failedCgiHandler();
    }
    //Initialize envp
    std::vector<std::string> envpData = buildEnvpData(data);
    //envpData was used for maintaining strings
    std::vector<char*> envp;
    for (size_t i = 0; i < envpData.size(); ++i)
        envp.push_back((char*)envpData[i].c_str());
    envp.push_back(NULL);

    char* argv[] = {
        (char *)data.interpreter.c_str(),
        (char *)data.scriptPath.c_str(),
        NULL
    };
    //Fork
    pid_t   pid;
    pid = fork();
    if (pid < 0){
        close(pipeIn[0]);
        close(pipeIn[1]);
        close(pipeOut[0]);
        close(pipeOut[1]);
        return failedCgiHandler();
    }
    if (pid == 0){
        dup2(pipeIn[0], STDIN_FILENO);
        dup2(pipeOut[1], STDOUT_FILENO);
        close(pipeIn[0]);
        close(pipeIn[1]);
        close(pipeOut[0]);
        close(pipeOut[1]);
        execve(data.interpreter.c_str(), argv, &envp[0]);
        exit(1);
    }
    else{        
        Client::CgiInfo cgiInfos;
        if (data.method == "POST" && !data.body.empty()){
            cgiInfos.pipeWrite = pipeIn[1];
        }
        else{
            cgiInfos.pipeWrite = -1;
            close(pipeIn[1]);   
        }
        cgiInfos.pipeRead = pipeOut[0];
        cgiInfos.bodyWrittenBytes = 0;
        cgiInfos.isCgi = true;
        cgiInfos.pid = pid;
        cgiInfos.start_time = time(NULL);
        close(pipeIn[0]);
        close(pipeOut[1]);
        return cgiInfos;
    }
    return failedCgiHandler();
}