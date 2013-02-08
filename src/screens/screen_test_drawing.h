#pragma once

#include "screen.h"
#include "screens/screen_manager.h"
#include "event/event.h"
#include <string>

class ScreenTestDrawing : public Screen
{
public:
	ScreenTestDrawing(std::string sName);
	virtual ~ScreenTestDrawing();
	
	void Init();
	
	void UpdateInternal(double delta);
	bool HandleEvent(const std::string &name, const IEvent &evt);
	
	void Draw(DrawBuffer db);
	
protected:
	const char *vss, *fss;
	unsigned /*vs, fs, id,*/ vao, buf[2], tex;
	std::string log;	
};
