#pragma once

#include <string>
#include "Stereoscopic.h"

struct WindowParams
{
	WindowParams() :
		width(0),
		height(0),
		fullscreen(0),
		fullscreen_monitor(0),
		samples(0),
		stereoscopic_mode(StereoscopicMode_None)
	{}

	int width;
	int height;

	int fullscreen;
	int fullscreen_monitor;
	int samples;
	StereoscopicMode stereoscopic_mode;
};

// designed as a thin wrapper around glfw.
class Window
{
public:
	virtual bool open(WindowParams, std::string) = 0;
	void close();
	bool is_focused();
	bool should_close();
	void *handle;
	WindowParams params;
};
