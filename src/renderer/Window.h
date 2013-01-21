#pragma once

#include <string>

struct WindowParams
{
	int width;
	int height;
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
