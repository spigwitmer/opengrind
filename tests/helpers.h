#include <string>
#include <physfs.h>

void init_physfs(const char* arg0)
{
	using namespace std;

	// Set the base dir for our VFS
	int err = PHYSFS_init(arg0);

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
	PHYSFS_mkdir("logs");

	// Make sure the logger is available everywhere.
	LOG = new Logger("logs/log.txt");

	if (!err)
		LOG->Error("Unable to initialize VFS");

	path = PHYSFS_getBaseDir();
	path += "Data/Shaders/";
}

void deinit_physfs()
{
	PHYSFS_deinit();
}