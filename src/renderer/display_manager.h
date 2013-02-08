#ifndef _DISPLAY_DISPLAYMANAGER_H_
#define _DISPLAY_DISPLAYMANAGER_H_

#define GLFW_NO_GLU
#include <GL/glfw3.h>

#include "utils/singleton.h"
#include "render_system.h"
#include "window.h"

#include <glm/glm.hpp>

class DisplayManager : public Singleton<DisplayManager>
{
public:
	void SetWindow(Nepgear::Window *window) { m_window = window; };
	Nepgear::Window* GetWindow() const { return m_window; }

	void SetRenderer(RenderSystem *renderer) { m_renderer = renderer; }
	RenderSystem* GetRenderer() const { return m_renderer; }

	/* This is the viewport for the window itself, unscaled. Get viewport
	 * from the renderer for the properly scaled one. */
	void SetViewport(glm::vec4 viewport) { m_viewport = viewport; }
	const glm::vec4 GetViewport() const { return m_viewport; }

private:
	Nepgear::Window *m_window;
	RenderSystem* m_renderer;
	glm::vec4 m_viewport;
};

#endif

/**
 * Colby Klein (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
