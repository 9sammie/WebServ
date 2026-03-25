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

std::string getStatus(std::string str){
    size_t lineStatus = str.find("Status:");
    size_t start = lineStatus + 7;
    std::string status = str.substr(start);
    // status.trim(' ');
    return status;
}

std::string cgiResultHandler(std::string result){
    std::string status = getStatus(result);
    std::string finalResponse;
    finalResponse = "HTTP/1.1 " + status;

}