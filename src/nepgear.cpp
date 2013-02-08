#include "nepgear.h"
#include "utils/logger.h"

#include <GLXW/glxw.h>
#include <GL/glfw3.h>

#include <vector>
#include <string>

// Man, does this look redundant.
namespace Nepgear
{

Nepgear::Nepgear(const int argc, char **argv, const char *log_file)
{
	using namespace std;

	// for convenience
	vector<string> vArgs;
	for (int i = 0; i<argc; i++)
		vArgs.push_back(string(argv[i]));

	Nepgear::Arg0 = argv[0];

	// Prepare VFS for use.
	File::init();

	// Make sure the logger is available everywhere.
	string log_path = "logs/";
	log_path += log_file;

	LOG = new Logger(log_path.c_str());

	// Scan arguments for any flags we must honor
	// TODO: Handle this somewhere else.
	for (size_t i = 0; i < vArgs.size(); i++)
	{
		if (vArgs[i] == "--no-colors")
			LOG->UseColors(false);
		if (vArgs[i] == "--show-traces")
			LOG->ShowTraces(true);
	}

	int err = glfwInit();
	if (!err)
		LOG->Error("glfw exploded");
}

Nepgear::~Nepgear()
{
	glfwTerminate();

	SAFE_DELETE(LOG);

	File::deinit();
}

}

/**
 * Colby Klein (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
