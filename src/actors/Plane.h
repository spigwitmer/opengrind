#pragma once

#include "ActorBase.h"

class Plane : public ActorBase
{
public:
	Plane();
	virtual ~Plane();

	void DrawInternal();

protected:
	// vertices + indices
	unsigned m_VBO[2];
	
	// make sure we don't draw if init failed.
	bool m_init;
};

/**
 * Colby Klein, Matt Vandermeulen (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
