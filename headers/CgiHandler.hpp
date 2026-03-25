#include "Client.hpp"
#include <vector>
#include <map>

typedef struct DataCgi{
    /*********************************************************/
    /*                 inside request/headers                */
    /*********************************************************/

    std::string method; //GET, POST, DELETE
    std::string serverProtocol;// HTTP/1.1
    std::string requestURI; // /cgi-bin/script.py/foo/bar?x=1
    std::string scriptName;// /cgi-bin/script.py
    std::string pathInfo;///foo/bar    extra path information that comes after the CGI script name in the URL
    std::string queryString; // x=1 part after ? in URI, first line of request
    std::map<std::string, std::string> headers;
    //type and length outside of headers to simplify envp building
    std::string contentType;
    std::string contentLength;
    std::string body;

    /*********************************************************/
    /*                 inside config file                    */
    /*********************************************************/

    std::string serverName;
    std::string scriptPath; //Absolute path of script
    std::string interpreter; //ex : /usr/bin/python3
    std::string scriptFilename; // equal to scriptPath but used by cgi-PHP
    std::string documentRoot;//where server files are located

    /*********************************************************/
    /*       during client/listenners initialisation         */
    /*********************************************************/

    std::string serverPort;// port on which a client is connected, stored inside each client
    std::string remoteAddr; //adress IP client
    std::string remotePort;//client port stored inside each client

    /*********************************************************/
    /*          just initialize it                           */
    /*********************************************************/

    std::string gateWayInterface;//version of the cgi CGI/1.1
    std::string redirectStatus;// used by php, usually 200, could crash if not present

    /*********************************************************/
    /*          Bonus                                        */
    /*********************************************************/
    // std::string authType;// authentication method used for the request such as Basic or Digest Would be updated during request parsing
    // std::string remoteUser;// username of the authenticated client Would be updated during acceptNewConnection()

}DataCgi;

Client::CgiInfo CgiHandler(DataCgi data);
std::vector<std::string> buildEnvpData(DataCgi data);