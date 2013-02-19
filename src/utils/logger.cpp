#include "logger.h"
#include "nepgear.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <physfs.h>

using namespace std;

Nepgear::Logger *LOG = NULL;

namespace Nepgear
{

int ng_vsnprintf(char *buf, size_t max, const char *in, va_list va)
{
	int need = vsnprintf(buf, max, in, va);

#ifdef _MSC_VER
	if (need < 0) need = _vscprintf(in, va);
#endif

	assert(need >= 0);

	return need;
}

/*
 * We'll need to do this in several functions and making it a normal function
 * doesn't behave properly. Macros to the rescue!
 */
#define FORMAT(str) \
{ \
	va_list va; \
	char staticbuf[1024]; \
	char *buf = staticbuf; \
	va_start(va, in); \
	int need = ng_vsnprintf(buf, sizeof(staticbuf), in, va) + 1; \
	if (need > sizeof(staticbuf)) \
	{ \
		/* staticbuf wasn't large enough, malloc large enough */ \
		buf = (char *) malloc(need); \
		va_start(va,in); \
		vsnprintf(buf, need, in, va); \
	} \
	va_end(va); \
	str = string(buf); \
	/* free if we had to malloc more space */ \
	if (buf != staticbuf) \
		free(buf); \
}

Logger::Logger(std::string path)
{
	string dir = path.substr(0, path.find_last_of("/"));

	m_File = new File(dir);
	m_File->mkdir();

	// Logger is borked if this happens
	if (!m_File->open(path, FileAccessMode_Write))
	{
		printf("%s\n", m_File->get_last_error().c_str());
		assert(0);
		return;
	}

	UseColors(true);
	ShowTraces(true);
#if DEBUG
	ShowDebug(true);
#else
	ShowDebug(false);
#endif

//	time_t raw = time(NULL);
//	char sTime[25];
//	strftime(sTime, 25, "%m/%d/%Y @ %H:%M:%S", localtime(&raw));

	Internal(string("# ") + Nepgear::FullName + " log started", "", FG_INVALID, false, false);
}

Logger::~Logger()
{
	Internal("# Log finished", "", FG_INVALID, false, false);

	m_File->close();
}

void GetTime(char *sTime)
{
	time_t raw = time(NULL);

	// Format the time (mm/dd hh:mm:ss) i.e. 12/30 06:24:50
	strftime(sTime, 25, "%H:%M:%S", localtime(&raw));
}

void Logger::Internal(string in, string message, ConsoleColor color, bool show, bool time)
{
	ostringstream stream;
	string str;
	char sTime[25];

	if (time)
		GetTime(sTime);
	else sTime[0] = '\0';

	stream << sTime << " ";
	if (message.length() > 1)
		stream << message << ": ";
	stream << in << "\n";
	str = stream.str();

	// Write out buffer to file
	m_File->write((void*)str.c_str(), str.length());

	// Write to console
	if (!show)
		return;

	// Rebuild message with colors
	stream.str("");

	if (color != FG_INVALID && m_use_colors)
	{
		stream << "\x1b[" << color << ";1m" << message << ":\x1b[0m ";
	}
	else
	{
		stream << message;
		if (message.length() > 1)
		{
			stream << ": ";
		}
	}

	stream << in << "\n";

	str = stream.str();

	printf("%s", str.c_str());
}

void Logger::Trace(const char *in, ...)
{
	string str;
	FORMAT(str);

	Internal(str, "", FG_INVALID, m_show_traces);
}

void Logger::Debug(const char *in, ...)
{
	string str;
	FORMAT(str);

	Internal(str, "Debug", FG_CYAN, m_show_debug);
}

void Logger::Warn(const char *in, ...)
{
	string str;
	FORMAT(str);

	Internal(str, "WARNING", FG_YELLOW);
}

void Logger::Error(const char *in, ...)
{
	string str;
	FORMAT(str);

	Internal(str, "ERROR", FG_RED);
}

}

/**
 * Colby Klein, Jarno Purontakanen, Matt Vandermeulen (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
