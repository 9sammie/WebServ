#include <unistd.h>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include "Client.hpp"
#include "CgiHandler.hpp"

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
    envpData.push_back("SERVER_PROTOCOL=" + data.serverProtocol);
    envpData.push_back("SCRIPT_NAME=" + data.scriptName);
    envpData.push_back("REQUEST_URI=" + data.requestURI);
    envpData.push_back("PATH_INFO=" + data.pathInfo);
    envpData.push_back("QUERY_STRING=" + data.queryString);
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


    envpData.push_back("SERVER_NAME=" + data.serverName);
    envpData.push_back("SCRIPT_FILENAME=" + data.scriptFilename);
    envpData.push_back("DOCUMENT_ROOT=" + data.documentRoot);
    envpData.push_back("SERVER_PORT=" + data.serverPort);
    envpData.push_back("REMOTE_ADDR=" + data.remoteAddr);
    envpData.push_back("REMOTE_PORT=" + data.remotePort);

    envpData.push_back("GATEWAY_INTERFACE=" + data.gateWayInterface);
    envpData.push_back("REDIRECT_STATUS=" + data.redirectStatus);
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