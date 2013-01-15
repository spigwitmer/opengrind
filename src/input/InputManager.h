#pragma once

#include "input/Inputs.h"
#include "input/ButtonState.h"
#include <glm/glm.hpp>

class Window;

class InputManager
{
public:
	InputManager();
	virtual ~InputManager();

	void DiscardQueue();

	void connect(Window*);

	void Update();

	// Grabs an input from the queue
	ButtonState *GetNextInput();

	// Single button testing
	ButtonState *GetButton(RSButtons b) const;

	// Simplistic mouse grabbing
	glm::ivec2 &GetMousePos() const;

private:
	Window *m_window;
};

/**
 * Colby Klein, Matt Vandermeulen (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
