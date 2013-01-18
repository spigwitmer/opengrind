#include "Nepgear.cpp"
#include "utils/File.cpp"
#include "utils/Logger.cpp"
#include "helpers.h"

int main(int argc, char **argv)
{
	Nepgear ng(argc, argv, "logs/vfs-test.txt");
	LOG->ShowTraces(true);

	File f("tests");
	if (f.is_dir())
		LOG->Trace("Found tests directory");

	LOG->Trace("Attempting to open tests/foo.txt...");

	f.open("tests/foo.txt");
	if (f.is_open()) {
		LOG->Trace("It worked!");
		LOG->Trace("File contents:\n" + f.read_string());
		f.close();
	} else {
		LOG->Trace("Couldn't open file. If game/tests/foo.txt exists, the VFS is broken.");
	}

	f.close();

	if (f.get_last_error().empty())
		LOG->Trace("File closed. All seems to be well");

	return 0;
}