#ifndef CGIRESPONSEPROCESSOR_HPP
#define CGIRESPONSEPROCESSOR_HPP

#include <string>
#include <map>
#include <sstream>
#include <ctime>
#include "Config.hpp"

std::string cgiResponseProcessor(std::string result, ServerConfig const & server);
void enforceHttpCompliance(std::map<std::string, std::string> & headersMap, std::string& body, ServerConfig& server);
std::string getBodySizeStr(std::string body);
std::string getDate();
std::map<std::string, std::string> getHeadersMaps(std::string& headers);
std::string getBody(std::string& str);
std::string getRawHeaders(std::string& str);
std::string strToLower(std::string& str);
std::string trim(std::string& str, const char* charset);

#endif