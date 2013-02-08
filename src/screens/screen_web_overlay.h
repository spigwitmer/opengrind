#pragma once

#include "screen.h"
#include "event/event.h"
#include <string>

namespace Awesomium
{
	class WebCore;
	class WebView;
}

class ScreenWebOverlay : public Screen
{
public:
	ScreenWebOverlay(std::string sName);
	virtual ~ScreenWebOverlay();
	
	void Init();
	
	void UpdateInternal(double delta);
	bool HandleEvent(const std::string &name, const IEvent &evt);
	
	void Draw(DrawBuffer db);
	
protected:
	Awesomium::WebCore* web_core;
	Awesomium::WebView* web_view;
	glm::vec4 viewport;
	ShaderProgram *wk_shader;
	unsigned tex, vao, buf[2];
	bool m_ready;
};
