#include <GLXW/glxw.h>

#include "nepgear.h"
#include "screens/screen_manager.h"
#include "input/input_manager.h"

// These two are singletons for convenience.
#include "event/event_manager.h"
#include "utils/logger.h"
#include "utils/string_utils.h"

#include "global.h"
#include <physfs.h>

// XXX
#include "renderer/display_manager.h"
#include "renderer/render_system.h"
#include "renderer/gl30/render_system_gl30.h"
#include "renderer/common/window_gl.h"

#include "lua/lua_manager.h"

using namespace std;

namespace Nepgear
{
const char *Nepgear::FullName = "OpenGrind";
const char *Nepgear::UnixName = "opengrind";
const char *Nepgear::Arg0;
}

class PrefsManager : public Singleton<PrefsManager>
{
public:
	int GetInteger(std::string section, std::string key)
	{
		// XXX: polyfill until I add this for real
		if (key == "Width")
			return 1280;
		else if (key == "Height")
			return 800;
		else if (key == "Fullscreen")
			return 0;
		return 0;
	}
};

static int run()
{
	using namespace Nepgear;

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
	p.stereoscopic_mode = StereoscopicMode_SBS;

	Window *window = new Window_GL();

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
		screen.Draw(p.stereoscopic_mode);

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
	Nepgear::Nepgear ng(argc, argv, "opengrind.log");
	return run();
}
