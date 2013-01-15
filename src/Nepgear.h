#pragma once

#include <vector>
#include <string>

// Helps prevent heap corruption.
#define SAFE_DELETE(p) if( (p) != NULL ) delete (p); (p) = NULL;

class Nepgear
{
public:
	Nepgear(std::vector<std::string> &vArguments);
	virtual ~Nepgear();

	int Run();
};

/**
 * Colby Klein (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
