#pragma once

#include <vector>
#include <string>
#include "global.h"

class Nepgear
{
public:
	Nepgear(std::vector<std::string> &vArguments);
	virtual ~Nepgear();

	int Run();

	static const char* Arg0;
	static const char* FullName;
	static const char* UnixName;
};

/**
 * Colby Klein (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
