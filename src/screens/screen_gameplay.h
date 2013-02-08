#pragma once

#include "screen.h"
#include "screens/screen_manager.h"
#include "event/event.h"
#include <string>

class ScreenGameplay : public Screen
{
public:
	ScreenGameplay(std::string sName);
	virtual ~ScreenGameplay();
	
	void Init();
	
	void UpdateInternal(double delta);
	bool HandleEvent(const std::string &name, const IEvent &evt);
	
	void Draw(DrawBuffer db);
};
