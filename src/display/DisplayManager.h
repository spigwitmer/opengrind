#ifndef _DISPLAY_DISPLAYMANAGER_H_
#define _DISPLAY_DISPLAYMANAGER_H_

#define GLFW_NO_GLU
#include <GL/glfw3.h>
#include <string>
#include <map>

#include "utils/Logger.h"
#include "utils/Singleton.h"
#include "renderer/RenderSystem.h"

class DisplayManager : public Singleton<DisplayManager>
{
public:
	DisplayManager();
	
	bool OpenWindow();
	void CloseWindow();
	GLFWwindow* GetWindow();
	bool IsFocused();

	bool RequestedClose();
	
	void Flush();
	
	void SetRenderer(RenderSystem *renderer) { m_renderer = renderer; }

	RenderSystem* GetRenderer() const { return m_renderer; }

private:
	GLFWwindow *m_window;
	std::map<GLenum, GLint> m_attribs;

	RenderSystem* m_renderer;
};

#endif

/**
 * Colby Klein (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
