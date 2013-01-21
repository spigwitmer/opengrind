#pragma once

#include "renderer/Window.h"

// Desktop OpenGL
class GLWindow : public Window
{
public:
	bool open(WindowParams, std::string title);
};
