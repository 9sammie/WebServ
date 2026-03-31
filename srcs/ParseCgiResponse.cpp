/*
    Le resultat du cgi peut contenir une page html ou non

    Il y a des headers, un status code
    potentiellement une page html
    ATTENTION, le cgi peut ne contenir que des \n sans le \r, penser a les ajouter s'ils manquent

    par etape,
    on recupere le status code pour formater la premiere ligne


    ex:Status-code : 404 Not Found 

    HTTP/1.1 404 Not Found
    Headers...
    Content-length: taille boy a calculer, ce header je dois l'ajouter moi meme

    body(html)











*/
#include <string>
#include <map>

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

// std::string getStatus(std::string& str){
//     size_t lineStatus = str.find("Status:");
//     if (lineStatus == std::string::npos)
//         return "200 OK\r\n";
//     size_t endStatusLine = str.find("\r\n", lineStatus);
//     size_t statusLineSize = endStatusLine - lineStatus;
//     size_t start = lineStatus + 7;
//     std::string status = str.substr(start, statusLineSize);
//     status = trim(status, "\t\r\n");
//     return status;  
// }

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

std::string cgiResultHandler(std::string result){
    // std::string status = getStatus(result);
    // std::string finalResponse;
    // finalResponse = "HTTP/1.1 " + status;

    std::string headers = getRawHeaders(result);
    if (headers.empty())
        /*PROBLEM*/;
    std::string body = getBody(result);
    std::map<std::string, std::string> headersMap = getHeadersMaps(headers);
}