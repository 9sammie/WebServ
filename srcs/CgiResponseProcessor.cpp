#include <string>
#include <map>
#include <sstream>
#include <ctime>
#include "Config.hpp"

std::string trim(std::string& str, const char* charset){
    size_t start = str.find_first_not_of(charset);
    if (start == std::string::npos)
        return "";
    size_t end = str.find_last_not_of(charset);
    return (str.substr(start, (end - start +1)));
}

std::string strToLower(std::string& str){
    for (size_t i = 0; i < str.size(); ++i){
        str[i] = std::tolower((unsigned char)str[i]);
    }
    return str;
}

std::string getRawHeaders(std::string& str){
    size_t emptyLine = str.find("\r\n\r\n");
    if (emptyLine == std::string ::npos)
        return "";
    return str.substr(0, emptyLine);
}

std::string getBody(std::string& str){
    size_t emptyLine = str.find("\r\n\r\n");
    if (emptyLine == std::string ::npos)
        return "";
    return str.substr(emptyLine + 4, str.size());
}

std::map<std::string, std::string> getHeadersMaps(std::string& headers){
    std::map<std::string, std::string> mapHeaders;
    size_t start = 0;
    size_t end;

    while ((end = headers.find("\r\n", start)) != std::string::npos){
        std::string currentLine = headers.substr(start, (end - start + 2));
        size_t currentStart = currentLine.find(":");
        if (currentStart == std::string::npos)
            continue ;
        std::string key = currentLine.substr(0, currentStart);
        std::string value = currentLine.substr(currentStart + 1);
        key = trim(key, " \t\r\n");
        key = strToLower(key);
        value = trim(value, " \t\r\n");
        mapHeaders[key] = value;
        start = end +2;
    }
    return mapHeaders;
}

std::string getDate(){
    // Expected format : Day, DD Mon YYYY HH:MM:SS GMT
    std::string strTime;
    std::stringstream ss;
    time_t actualTime = time(NULL);
    struct tm *tmTime = gmtime(&actualTime);
    if (tmTime == NULL)
        return "";
    char buffer[100];
    if (strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", tmTime) == 0)
        return "";
    return buffer;
}

std::string getBodySizeStr(std::string body){
    size_t size = body.size();
    std::stringstream ss;
    ss << size;
    return ss.str();
}

void enforceHttpCompliance(std::map<std::string, std::string> & headersMap, std::string& body, ServerConfig& server){
    // DATE, SERVER, CONNECTION, STATUS, CONTENTLENGTH
    headersMap.erase("status");
    headersMap["server"] = "Rats_du_port_80";
    headersMap["date"] = getDate();
    headersMap["content-length"] = getBodySizeStr(body);
    if (server.hasKeepalive == true)
        headersMap["connection"] = "keep-alive";
    else
        headersMap["connection"] = "close";
}

std::string cgiResultHandler(std::string result, ServerConfig& server){
    std::string cgiResponse;
    std::string headers = getRawHeaders(result);
    if (headers.empty())
        return "HTTP/1.1 502 Bad Gateway\r\nContent-Length: 0\r\n\r\n";
    std::string body = getBody(result);
    std::map<std::string, std::string> headersMap = getHeadersMaps(headers);
    std::map<std::string, std::string>::iterator itS = headersMap.find("status");
    std::map<std::string, std::string>::iterator itL = headersMap.find("location");
    if (itS == headersMap.end() && itL == headersMap.end())
        cgiResponse += "HTTP/1.1 200 OK\r\n";
    else if (itS == headersMap.end() && itL != headersMap.end())
        cgiResponse += "HTTP/1.1 302 Found\r\n";
    else
        cgiResponse += "HTTP/1.1 " + itS->second + "\r\n";
    enforceHttpCompliance(headersMap, body, server);
    for (std::map<std::string, std::string>::iterator it = headersMap.begin(); it != headersMap.end(); ++it){
        cgiResponse += it->first + ": " + it->second + "\r\n";
    }
    cgiResponse += "\r\n";
    if (!body.empty())
        cgiResponse += body;
    return cgiResponse;
}