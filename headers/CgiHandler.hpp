#include "Client.hpp"
#include <vector>

struct DataCgi{};

Client::CgiInfo CgiHandler(DataCgi data);
std::vector<std::string> buildEnvpData(DataCgi data);