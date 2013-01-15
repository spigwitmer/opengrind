/* Logger - Simply logs activity as the owner wishes */
#pragma once

#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <physfs.h>
#include <string>

// Extra warnings, hm
#define PRINTF(m,n) __attribute__((format(__printf__,m,n)))

enum LogLevel
{
	LOG_TRACE	= 0x01,
	LOG_DEBUG	= 0x02,
	LOG_WARN	= 0x04,
	LOG_ERROR	= 0x08
};

enum ConsoleColor
{
	FG_BLACK	= 30,
	FG_RED		= 31,
	FG_GREEN	= 32,
	FG_BROWN	= 33,
	FG_BLUE		= 34,
	FG_MAGENTA	= 35,
	FG_CYAN		= 36,
	FG_WHITE	= 37,
	FG_YELLOW	= 38,
	FG_INVALID	= 255
};

class Logger
{
public:
	// Constructor and destructor
	Logger( const char *sPath );
	virtual ~Logger();
	
	// Log functions
	void Trace(const char *fmt, ...) PRINTF(2,3);
	void Debug(const char *fmt, ...) PRINTF(2,3);
	void Warn(const char *fmt, ...) PRINTF(2,3);
	void Error(const char *fmt, ...) PRINTF(2,3);
	
	void UseColors(bool flag) { m_use_colors = flag; }
	void ShowTraces(bool flag) { m_show_traces = flag; }

private:
	// This one does all the real work
	void Internal(std::string str, std::string message, ConsoleColor color, bool show = true);
	
	// The file for this log
	PHYSFS_File *m_File;
	bool m_use_colors, m_show_traces;
};

// Convenience throughout the program, heh
extern Logger *LOG;
