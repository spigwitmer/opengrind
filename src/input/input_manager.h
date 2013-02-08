#pragma once

#include "input/inputs.h"
#include "input/button_state.h"
#include "utils/singleton.h"
#include <glm/glm.hpp>


namespace Nepgear
{
class Window;
}

class InputManager : public Singleton<InputManager>
{
public:
	InputManager();
	virtual ~InputManager();

	void DiscardQueue();

	void connect(Nepgear::Window*);

	void Update();

	// Grabs an input from the queue
	ButtonState *GetNextInput();

	// Single button testing
	ButtonState *GetButton(RSButtons b) const;

	// Simplistic mouse grabbing
	glm::ivec2 &GetMousePos() const;

private:
	Nepgear::Window *m_window;
};

/**
 * Colby Klein, Matt Vandermeulen (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
