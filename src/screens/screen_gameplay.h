#pragma once

#include "screen.h"
#include "screens/screen_manager.h"
#include "event/event.h"
#include <string>
#include "input/input_manager.h"

class ScreenGameplay : public Screen
{
public:
	ScreenGameplay(std::string sName);
	virtual ~ScreenGameplay();
	
	void Init();
	
	void Input(Nepgear::InputManager*);

	void UpdateInternal(double delta);
	bool HandleEvent(const std::string &name, const IEvent &evt);
	
	void Draw(DrawBuffer db);
};
