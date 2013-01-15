#include <GLXW/glxw.h>
#include <GL/glfw3.h>
#include "DisplayManager.h"

void Window::close()
{
	glfwDestroyWindow((GLFWwindow*)handle);
}

bool Window::is_focused()
{
	return (bool)glfwGetWindowParam((GLFWwindow*)handle, GLFW_FOCUSED);
}

bool Window::should_close()
{
	return (bool)glfwGetWindowParam((GLFWwindow*)handle, GLFW_SHOULD_CLOSE);
}

/**
 * Colby Klein (c) 2013
 * Licensed under the terms of the MIT license. See License.txt.
 */
