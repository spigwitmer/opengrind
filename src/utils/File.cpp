#include "File.h"
#include "Nepgear.h"
#include "utils/Logger.h"

#include <physfs.h>
#include <cassert>
#include <cstring>

using namespace std;

File::File(string path, FileAccessMode fm)
{
	filename = path;
	mode = fm;
	handle = NULL;

	if (!PHYSFS_isInit())
	{
		int success = 0;
		string tmp, save_folder = string(".") + Nepgear::UnixName;

		// Set the base dir for the VFS
		success = PHYSFS_init(Nepgear::Arg0);
		if (!success) goto _File_h_physfs_error;

		/*
		 * Set up some hopefully sane defaults:
		 * Mount /game as the root for file access
		 * Create ~/.Nepgear::UnixName for file writing, mount it.
		 * Create logs folder.
		 */
		PHYSFS_permitSymbolicLinks(1);
		tmp = PHYSFS_getBaseDir();
		tmp = tmp.substr(0, tmp.find_last_of("/"));
		tmp = tmp.substr(0, tmp.find_last_of("/"));
		tmp += "/game";
		PHYSFS_mount(tmp.c_str(), NULL, 1);

		tmp = PHYSFS_getUserDir();

		// Set write dir to home and create save folder if it doesn't exist
		success = PHYSFS_setWriteDir(tmp.c_str());
		success = PHYSFS_mkdir(save_folder.c_str());
		if (!success) goto _File_h_physfs_error;

		// Change write dir to our save folder
		tmp += save_folder;
		success = PHYSFS_setWriteDir(tmp.c_str());
		if (!success) goto _File_h_physfs_error;

		PHYSFS_mount(tmp.c_str(), NULL, 0);
		if (success) goto _File_h_physfs_error;

		// Make log folder
		PHYSFS_mkdir("logs");

		if (!success) return;

		_File_h_physfs_error:
		{
			string err_log = get_last_error();
			if (!err_log.empty())
			{
				if (LOG)
					LOG->Warn(err_log);
				else
					printf("%s\n", err_log.c_str());
			}
		}
	}
}

bool File::mkdir()
{
	return (bool)PHYSFS_mkdir(filename.c_str());
}

bool File::open(string path, FileAccessMode fm)
{
	if (handle)
		return false;

	if (!path.empty())
		filename = path;
	if (fm != FileAccessMode_Invalid)
		mode = fm;

	switch(mode)
	{
		case FileAccessMode_Read:
			handle = PHYSFS_openRead(filename.c_str());
			break;
		case FileAccessMode_Write:
			handle = PHYSFS_openWrite(filename.c_str());
			break;
		case FileAccessMode_Append:
			handle = PHYSFS_openAppend(filename.c_str());
			break;
		default: assert(0); break;
	}

	string err = get_last_error();
	if (err.empty())
		return true;

	return (bool)handle;
}

bool File::close()
{
	if (!is_open())
		return true;
	flush();
	return (bool)PHYSFS_close((PHYSFS_File*)handle);
}

size_t File::length()
{
	if (!is_open())
		return 0;
	return PHYSFS_fileLength((PHYSFS_File*)handle);
}

bool File::flush()
{
	if (!is_open())
		return 0;
	return (bool)PHYSFS_flush((PHYSFS_File*)handle);
}

bool File::read(char *buf, size_t bytes, size_t *got)
{
	if (!is_open())
	{
		assert(0);
		return false;
	}
	int fetched;
	fetched = PHYSFS_read((PHYSFS_File*)handle, buf, 1, bytes);

	if (got) *got = fetched;

	string err = get_last_error();
	if (err.empty())
		return true;

	LOG->Debug("[%s] %s", filename.c_str(), err.c_str());

	return false;
}

bool File::write(void* data, size_t bytes)
{
	return (bool)PHYSFS_write((PHYSFS_File*)handle, data, bytes, 1);
}

bool File::is_dir()
{
	return (bool)PHYSFS_isDirectory(filename.c_str());
}

string File::get_real_path()
{
	return string(PHYSFS_getRealDir(filename.c_str())) + filename;
}

string File::get_last_error()
{
	string ret;
	const char *err = PHYSFS_getLastError();
	if (err && err != last_error)
	{
		last_error = err;
		ret += last_error;
	}

	return ret;
}
