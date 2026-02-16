#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <vector>

class Config
{
	private:
	std::vector<std::string> _servers;

	public:
	void parseFile(const std::string& path);
	const std::vector<std::string>& getServers() const;

};

#endif