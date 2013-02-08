#pragma once

#include "renderer/window.h"

namespace Nepgear
{
// Desktop OpenGL
class Window_GL : public Window
{
public:
	bool open(WindowParams, std::string title);
};
}
