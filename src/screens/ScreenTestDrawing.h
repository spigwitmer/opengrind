#pragma once

#include "Screen.h"
#include "screens/ScreenManager.h"
#include "events/Event.h"
#include <string>

class ScreenTestDrawing : public Screen
{
public:
	ScreenTestDrawing(std::string sName);
	virtual ~ScreenTestDrawing();
	
	void Init();
	
	void UpdateInternal(double delta);
	void HandleMessage(const std::string &name, const IEvent &evt);
	
	void Draw();
	
protected:
	const char *vss, *fss;
	unsigned /*vs, fs, id,*/ vao, buf[2], tex;
	std::string log;	
};
