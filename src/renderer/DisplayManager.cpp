#include <GLXW/glxw.h>
#include <GL/glfw3.h>
#include "DisplayManager.h"

namespace Nepgear
{

void Window::close()
{
	glfwDestroyWindow((GLFWwindow*)handle);
}

bool Window::is_focused()
{
	return glfwGetWindowParam((GLFWwindow*)handle, GLFW_FOCUSED) != 0;
}

bool Window::should_close()
{
	return glfwGetWindowParam((GLFWwindow*)handle, GLFW_SHOULD_CLOSE) != 0;
}

}

/**
 * Colby Klein (c) 2013
 * Licensed under the terms of the MIT license. See License.txt.
 */
