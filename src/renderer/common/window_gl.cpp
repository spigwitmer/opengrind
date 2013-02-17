#include "window_gl.h"
#include "utils/logger.h"

#include <GLXW/glxw.h>
#include <GL/glfw3.h>

namespace Nepgear
{

bool Window_GL::open(WindowParams p, std::string title)
{
//	glfwWindowHint(GLFW_SAMPLES, p.samples);
	glfwWindowHint(GLFW_RESIZABLE, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	int count;
	GLFWmonitor **ml = glfwGetMonitors(&count);
	GLFWmonitor *m = NULL;

	if (p.fullscreen)
	{
		m = glfwGetPrimaryMonitor();
		if (p.fullscreen_monitor && p.fullscreen_monitor <= count)
			m = ml[p.fullscreen_monitor-1];
	}

	handle = (void*)glfwCreateWindow(p.width, p.height, title.c_str(), m, NULL);
	glfwMakeContextCurrent((GLFWwindow*)handle);

	params = p;

	// Make sure we were able to create a rendering context.
	if (!handle)
	{
		LOG->Error("Unable to obtain an OpenGL 3.1 rendering context.");
		return false;
	}
	if (glxwInit() != 0)
		LOG->Error("Unable to load required OpenGL extensions.");

	return true;
}

}