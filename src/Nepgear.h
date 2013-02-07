#pragma once

#include "global.h"

namespace Nepgear
{

class Nepgear
{
public:
	Nepgear(const int argc, char **argv, const char* log_file);
	virtual ~Nepgear();

	int Run();

	static const char* Arg0;
	static const char* FullName;
	static const char* UnixName;
};

}

/**
 * Colby Klein (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
