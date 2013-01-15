#ifndef _DISPLAY_DISPLAYMANAGER_H_
#define _DISPLAY_DISPLAYMANAGER_H_

#define GLFW_NO_GLU
#include <GL/glfw3.h>

#include "utils/Singleton.h"
#include "RenderSystem.h"
#include "Window.h"

class DisplayManager : public Singleton<DisplayManager>
{
public:
	void SetWindow(Window* window) { m_window = window; };
	Window* GetWindow() const { return m_window; }

	void SetRenderer(RenderSystem *renderer) { m_renderer = renderer; }
	RenderSystem* GetRenderer() const { return m_renderer; }

private:
	Window *m_window;
	RenderSystem* m_renderer;
};

#endif

/**
 * Colby Klein (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
