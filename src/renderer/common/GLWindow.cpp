#include "GLWindow.h"
#include "utils/Logger.h"

#include <GLXW/glxw.h>
#include <GL/glfw3.h>

bool GLWindow::open(WindowParams p, std::string title)
{
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_RESIZABLE, 1);

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
		LOG->Error("Unable to obtain an OpenGL 3.0 rendering context.");
		return false;
	}
	if (glxwInit() != 0)
		LOG->Error("Unable to load required OpenGL extensions.");

	return true;
}
