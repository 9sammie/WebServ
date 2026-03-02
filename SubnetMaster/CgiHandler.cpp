#include <unistd.h>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>

typedef struct DataCgi{
    std::string method; //GET, POST, DELETE
    std::string URI; // /cgi-bin/cgi.py
    std::string queryString; //part after ? in URI, first line of request
    std::map<std::string, std::string> headers;
    std::string body;
    std::string scriptPath; //Absolute path of script
    std::string interpreter; //ex : /usr/bin/python3
}DataCgi;

int CgiHandler(DataCgi data){
    //Initialize pipes
    int     pipeIn[2];
    int     pipeOut[2];

    if (pipe(pipeIn) < 0)
        return -1;
    if (pipe(pipeOut) < 0){
        close(pipeIn[0]);
        close(pipeIn[1]);
        return -1;
    }
    
    //Initialize envp
    std::vector<std::string> envpData;
    envpData.clear();
    envpData.push_back("REQUEST_METHOD=" + data.method);
    envpData.push_back("QUERY_STRING=" + data.queryString);
    envpData.push_back("SCRIPT_NAME=" + data.URI);
    envpData.push_back("GATEWAY_INTERFACE=CGI/1.1");
    envpData.push_back("SERVER_PROTOCOL=HTTP/1.1");

    for (std::map<std::string, std::string>::const_iterator it = data.headers.begin(); it != data.headers.end(); ++it){
         std::string key = it->first;
         if (key == "content-type" || key == "content-length")
            continue;
        for(size_t i = 0; i <key.size(); ++i){
            if (key[i] == '-')
                key[i] = '_';
            key[i] = std::toupper((unsigned char)key[i]);
        }
        envpData.push_back("HTTP_" + key + "=" + it->second);
    }
    //envpData was used for maintaining strings
    std::vector<char*> envp;
    for (size_t i =0; i <envpData.size(); ++i)
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
        return -1;
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
        if (data.method == "POST" && !data.body.empty())
            write(pipeIn[1], data.body.c_str(), data.body.size());
        close(pipeIn[1]);
        close(pipeIn[0]);
        close(pipeOut[1]);
        return pipeOut[0];
    }
    return -1;
}