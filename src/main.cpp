#include <GLXW/glxw.h>

#include "Nepgear.h"
#include "screens/ScreenManager.h"
#include "input/InputManager.h"

// These two are singletons for convenience.
#include "events/EventManager.h"
#include "utils/Logger.h"
#include "utils/StringUtils.h"

#include "global.h"
#include <physfs.h>

// XXX
#include "renderer/DisplayManager.h"
#include "renderer/RenderSystem.h"
#include "renderer/gl30/RenderSystem_GL30.h"
#include "renderer/common/GLWindow.h"

#include "lua/LuaManager.h"

using namespace std;

const char *Nepgear::FullName = "Nepgear";
const char *Nepgear::UnixName = "nepgear";
const char *Nepgear::Arg0;

static int run()
{
	ScreenManager screen;
	DisplayManager *display = DisplayManager::GetSingleton();
	InputManager *input = InputManager::GetSingleton();

	RenderSystem *renderer = new RenderSystem_GL30();
	WindowParams p { 960, 540 };
	Window *window = new GLWindow();

	display->SetRenderer(renderer);
	display->SetWindow(window);
	screen.SetRenderer(renderer);

	if (!window->open(p, "OpenGrind"))
		return 1;

	display->SetViewport(glm::vec4(0, 0, p.width, p.height));
	renderer->UpdateViewport();

	input->connect(window);

	renderer->Init();

	// Test!
	string dir = "/themes/default/";
	LuaManager *lua = LuaManager::GetSingleton();

	char **list = PHYSFS_enumerateFiles(dir.c_str());
	for (int i = 0; list[i] != NULL; i++)
	{
		// Expand path
		string file = dir + list[i];

		// Get extension and filter out dirs and non-lua files.
		string ext = utils::chop(file, ".", true);

		File f(file);
		if (f.is_dir() || ext != "lua")
			continue;

		// Skip if we can't open the thing.
		if (!f.open())
			continue;

		// Read lua file into buffer and run it
		size_t length = f.length();
		char  *buffer = new char[length];

		f.read(buffer, length);
		f.close();

		// XXX: ext is appended to this... nasty.
		buffer[length] = '\0';

		lua->Load(buffer, length, list[i]);

		delete[] buffer;
	}

	screen.PushScreen("ScreenTest");

	while (!window->should_close())
	{
		if (unlikely(input->GetButton(RS_KEY_ESC)->IsDown()))
			break;

		screen.Update(window->is_focused());
		screen.Draw();

		glfwSwapBuffers((GLFWwindow*)window->handle);

		// Break if user closed the window
		input->Update();
	}

	window->close();

	delete window;
	delete renderer;

	return 0;
}

int main(int argc, char **argv)
{
	Nepgear ng(argc, argv, "opengrind.log");
	return run();
}
