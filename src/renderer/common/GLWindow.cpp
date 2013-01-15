#include "GLWindow.h"
#include "utils/Logger.h"

#include <GLXW/glxw.h>
#include <GL/glfw3.h>

bool GLWindow::open(WindowParams p)
{
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_RESIZABLE, 1);

	handle = (void*)glfwCreateWindow(p.width, p.height, "", NULL, NULL);
	glfwMakeContextCurrent((GLFWwindow*)handle);

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
