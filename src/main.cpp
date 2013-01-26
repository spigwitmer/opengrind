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

const char *Nepgear::FullName = "OpenGrind";
const char *Nepgear::UnixName = "opengrind";
const char *Nepgear::Arg0;

class PrefsManager : public Singleton<PrefsManager>
{
public:
	int GetInteger(std::string section, std::string key)
	{
		// XXX: polyfill until I add this for real
		if (key == "Width")
			return 1280;
		else if (key == "Height")
			return 720;
		else if (key == "Fullscreen")
			return 0;
		return 0;
	}
};

static int run()
{
	ScreenManager screen;

	// XXX: I don't like singletons, but I like actually getting things done.
	DisplayManager	*display = DisplayManager::GetSingleton();
	InputManager	*input   = InputManager::GetSingleton();
	LuaManager		*lua     = LuaManager::GetSingleton();
	PrefsManager	*prefs   = PrefsManager::GetSingleton();

	RenderSystem *renderer = new RenderSystem_GL30();
	WindowParams p;
	p.width = prefs->GetInteger("Display", "Width");
	p.height = prefs->GetInteger("Display", "Height");
	p.fullscreen = prefs->GetInteger("Display", "Fullscreen");
	p.fullscreen_monitor = prefs->GetInteger("Display", "FullscreenMonitor");

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

	char **list = PHYSFS_enumerateFiles(dir.c_str());
	for (int i = 0; list[i] != NULL; i++)
	{
		string file = dir + list[i]; // full path.

		// We don't need dirs or anything but Lua files, here.
		string ext = utils::chop(file, ".", true);

		File f(file);
		if (f.is_dir() || ext != "lua")
			continue;

		// Skip if we can't open the thing.
		if (f.open())
		{
			lua->Load(f.read_string(), list[i]);
			f.close();
		}
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
