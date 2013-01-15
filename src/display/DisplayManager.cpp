#include <GLXW/glxw.h>
#include <GL/glfw3.h>
#include <string>

#include "renderer/common/Error.h"

#include "DisplayManager.h"
#include "utils/Logger.h"

using namespace std;

DisplayManager::DisplayManager() : m_window(0)
{
}

void DisplayManager::Flush()
{
	glfwSwapBuffers(m_window);
}

GLFWwindow *DisplayManager::GetWindow()
{
	return m_window;
}

bool DisplayManager::RequestedClose()
{
	return (bool)glfwGetWindowParam(m_window, GLFW_SHOULD_CLOSE);
}

bool DisplayManager::OpenWindow()
{
	// Shared window params, we always want these.
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_RESIZABLE, 1);
	//glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	LOG->Trace("Using Renderer: %s", m_renderer->GetName().c_str());

	/*
	 * TODO:
	 * - Use preferences.
	 * - Detect available renderers.
	 * - Have renderers properly initialize themselves.
	 * - Don't tie into OpenGL. it should be possible to use any
	 *   other rendering API, should you want to implement it.
	 *   (future)
	 */
	if (m_renderer->GetName() == "GL30")
	{
		// First, try to create a GL 3.2 context
		//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
		//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 0);
		m_window = glfwCreateWindow(854, 480, "Rhythm-Station", NULL, NULL);

		// Make sure we were able to create a rendering context.
		if (!m_window)
		{
			LOG->Error("Unable to obtain an OpenGL 3.0 rendering context.");
			return false;
		}
		if (glxwInit() != 0)
			LOG->Error("Kaboom!");
	}
	else
	{
		LOG->Debug("Invalid renderer.");
	}

	// make sure nothing is terribly wrong from the start.
	CheckError();

	return true;
}

void DisplayManager::CloseWindow()
{
	if (m_window)
		glfwDestroyWindow(m_window);
}

bool DisplayManager::IsFocused()
{
	return (bool)glfwGetWindowParam(m_window, GLFW_FOCUSED);
}

/**
 * Colby Klein (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
