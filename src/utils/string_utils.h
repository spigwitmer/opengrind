#pragma once

#include <vector>
#include <string>

namespace utils
{
	std::vector<std::string> split(std::string &str, char delim);
	std::string chop(std::string str, std::string search, bool reverse = false);
	void trim(std::string &str);
}
