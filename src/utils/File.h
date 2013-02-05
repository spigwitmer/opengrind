#pragma once

#include <string>
#include <vector>

enum FileAccessMode
{
	FileAccessMode_Read = 0,
	FileAccessMode_Write,
	FileAccessMode_Append,
	FileAccessMode_Invalid
};

class File
{
public:
	File(std::string path = "", FileAccessMode fm = FileAccessMode_Read);
	virtual ~File();

	bool mkdir(); // fails if the path is not writable.

	bool open(std::string path = "", FileAccessMode fm = FileAccessMode_Invalid);
	bool close(); // this CAN fail if writes were buffered

	bool seek(size_t pos);
	size_t tell();

	bool read(char *buf, size_t obj_count, size_t obj_size = 1);
	std::string read_string(size_t start = 0, size_t end = 0);
	size_t get_last_read() const { return last_read; }

	bool write(void *data, size_t bytes);
	bool flush();

	size_t length();
	bool is_dir();
	bool is_open() const { return handle != NULL; };

	std::string get_real_path();
	std::string get_last_error();

	static void init();
	static void deinit();

protected:
	const char *last_error;
	size_t last_read;
	std::string filename;
	void *handle; // PhysFS file handle
	FileAccessMode mode;
};
