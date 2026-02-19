#include "Config.hpp"

int main(int argc, char **argv)
{
	if (argc!= 2)
		return(printf("Error: expected arguments: ./webser <configFile>\n"), 1);
	Config *config = new(Config);

	config->parseFile(argv[1]);
}