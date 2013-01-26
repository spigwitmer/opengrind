#include "File.h"
#include "Nepgear.h"
#include "utils/Logger.h"

#include <physfs.h>
#include <cassert>
#include <cstring>

using namespace std;

void File::init()
{
	assert(!PHYSFS_isInit());

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
		string err_log;
		const char *err = PHYSFS_getLastError();
		if (err)
			err_log = err;

		if (!err_log.empty())
			printf("%s\n", err_log.c_str());
	}
}

void File::deinit()
{
	assert(PHYSFS_isInit());
	PHYSFS_deinit();
}

File::File(string path, FileAccessMode fm)
{
	filename = path;
	mode = fm;
	handle = NULL;
}

File::~File()
{
	close();
}

bool File::mkdir()
{
	return PHYSFS_mkdir(filename.c_str()) != 0;
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

	return handle != 0;
}

bool File::close()
{
	if (!is_open())
		return true;
	flush();
	return PHYSFS_close((PHYSFS_File*)handle) != 0;
}

size_t File::length()
{
	if (!is_open())
		return 0;
	return (size_t)PHYSFS_fileLength((PHYSFS_File*)handle);
}

bool File::flush()
{
	if (!is_open())
		return 0;
	return PHYSFS_flush((PHYSFS_File*)handle) != 0;
}

bool File::seek(size_t pos)
{
	if (!is_open())
		return 0;
	return PHYSFS_seek((PHYSFS_File*)handle, pos) != 0;
}

bool File::read(char *buf, size_t bytes, size_t *got)
{
	if (!is_open())
	{
		assert(0);
		return false;
	}
	size_t fetched;
	fetched = (size_t)PHYSFS_read((PHYSFS_File*)handle, buf, 1, bytes);

	if (got) *got = fetched;

	string err = get_last_error();
	if (err.empty())
		return true;

	LOG->Debug("[%s] %s", filename.c_str(), err.c_str());

	return false;
}

string File::read_string(size_t start, size_t end)
{
	size_t length = this->length();
	char* buf = new char[length];

	if (end > length)
		LOG->Warn("Attempted to seek past end of file!");
	if (end <= start || end == 0)
		end = length;

	seek(start);
	read(buf, end - start);
	seek(0);

	// make sure we've got a null terminator
	buf[length] = '\0';

	string ret = buf;
	delete[] buf;

	return ret;
}

bool File::write(void* data, size_t bytes)
{
	return PHYSFS_write((PHYSFS_File*)handle, data, bytes, 1) != 0;
}

bool File::is_dir()
{
	return PHYSFS_isDirectory(filename.c_str()) != 0;
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
