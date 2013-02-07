#pragma once

#include "renderer/Window.h"

namespace Nepgear
{
// Desktop OpenGL
class GLWindow : public Window
{
public:
	bool open(WindowParams, std::string title);
};
}
