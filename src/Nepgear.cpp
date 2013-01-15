#include <GLXW/glxw.h>

#include "Nepgear.h"
#include "screens/ScreenManager.h"
#include "input/InputManager.h"

// These two are singletons for convenience.
#include "events/EventManager.h"
#include "utils/Logger.h"

#include "global.h"
#include <physfs.h>

// XXX
#include "renderer/DisplayManager.h"
#include "renderer/RenderSystem.h"
#include "renderer/gl30/RenderSystem_GL30.h"

#include "lua/LuaManager.h"

//#include <cstring>

using namespace std;

// physfs debug helpers
#define CHECK_ERROR(hint) {\
	while (const char* err = PHYSFS_getLastError())\
	{\
		if (LOG) \
			LOG->Warn("%s: %s", hint, err);\
		else \
			printf("%s: %s\n", hint, err);\
	}\
}

#define LIST(f) {\
	LOG->Debug("listing %s", f);\
	char **list = PHYSFS_enumerateFiles(f);\
	for (int i = 0; list[i] != NULL; i++)\
	{\
		LOG->Debug("%s", list[i]);\
	}\
}

Nepgear::Nepgear(vector<string> &vArgs)
{
	int err = 0;

	// Set the base dir for our VFS
	err = PHYSFS_init(vArgs[0].c_str());

	PHYSFS_permitSymbolicLinks(1);
	string path = PHYSFS_getBaseDir();
	path = path.substr(0, path.find_last_of("/"));
	path = path.substr(0, path.find_last_of("/"));
	path += "/game";
	PHYSFS_mount(path.c_str(), NULL, 1);

	path = PHYSFS_getUserDir();

	// Default to standard unix path
	string save_folder = ".nepgear";

	// Set write dir to home and create save folder if it doesn't exist
	PHYSFS_setWriteDir(path.c_str());
	PHYSFS_mkdir(save_folder.c_str());

	// Change write dir to our save folder
	path += save_folder;
	PHYSFS_setWriteDir(path.c_str());

	PHYSFS_mount(path.c_str(), NULL, 0);
	CHECK_ERROR("Initializing VFS");

	// Make log folder
	PHYSFS_mkdir("logs");

	// Make sure the logger is available everywhere.
	LOG = new Logger("logs/log.txt");

	if (!err)
		LOG->Error("Unable to initialize VFS");

	// Scan arguments for any flags we must honor
	// TODO: Handle this somewhere else.
	for (size_t i = 0; i < vArgs.size(); i++)
	{
		if (vArgs[i] == "--no-colors")
			LOG->UseColors(false);
		if (vArgs[i] == "--show-traces")
			LOG->ShowTraces(true);
	}

	path = PHYSFS_getBaseDir();
	path += "Data/Shaders/";

	err = glfwInit();
	if (!err)
		LOG->Error("glfw exploded");
}
#undef LIST
#undef CHECK_ERROR

Nepgear::~Nepgear()
{
	SAFE_DELETE(LOG);

	PHYSFS_deinit();
}

#include "renderer/common/GLWindow.h"

int Nepgear::Run()
{
	ScreenManager screen;
	DisplayManager display;
	InputManager input;

	RenderSystem *renderer = new RenderSystem_GL30();
	WindowParams p { 960, 540 };
	Window *window = new GLWindow();


	display.SetRenderer(renderer);
	if (!window->open(p))
		return 1;

	input.connect(window);

	renderer->Init();

	screen.SetRenderer(renderer);

	// Test!
	string dir = "themes/default/";
	PHYSFS_File *hnd = NULL;
	PHYSFS_sint64 file_size = 0;
	LuaManager *lua = LuaManager::GetSingleton();

	char *buffer;

	char **list = PHYSFS_enumerateFiles(dir.c_str());
	for (int i = 0; list[i] != NULL; i++)
	{
		// Expand path
		string file = dir + list[i];

		// Get extension and filter out dirs and non-lua files.
		size_t pos = file.find_last_of(".");
		string ext = "";

		if (pos != string::npos)
			ext = file.substr(pos, file.length());

		if (PHYSFS_isDirectory(file.c_str()) || ext != ".lua")
			continue;

		// Read lua file into buffer and run it
		hnd			= PHYSFS_openRead(file.c_str());
		file_size	= PHYSFS_fileLength(hnd);
		buffer		= new char[file_size];

		PHYSFS_read(hnd, buffer, file_size, 1);
		buffer[file_size] = '\0'; // XXX: .lua is appended to the buffer?

		lua->Load(buffer, file_size, list[i]);

		delete[] buffer;
	}

	screen.PushScreen("ScreenTest");

	while (!window->should_close())
	{
		if (unlikely(input.GetButton(RS_KEY_ESC)->IsDown()))
			break;

		screen.Update();
		screen.Draw();

		glfwSwapBuffers((GLFWwindow*)window->handle);

		// Break if user closed the window
		input.Update();
	}

	// Don't bother broadcasting messages after the window closes.
	EventManager::ClearAll();

	window->close();

	return 0;
}

int main(int argc, char **argv)
{
	// Build arguments into a vector before passing control
	vector<string> args;
	for (int i = 0; i<argc; i++)
		args.push_back(string(argv[i]));

	Nepgear rs(args);
	return rs.Run();
}

/**
 * Colby Klein (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
