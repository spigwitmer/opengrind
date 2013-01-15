#include "Logger.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <cstdlib>

Logger *LOG = NULL;

using namespace std;

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
	unsigned int need = vsnprintf(buf, sizeof(staticbuf), in, va) + 1; \
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

Logger::Logger(const char *sPath)
{
	m_File = PHYSFS_openWrite(sPath);

	// Logger is borked if this happens
	if (m_File == NULL)
	{
		printf("%s\n", PHYSFS_getLastError());
		assert(0);
		return;
	}

	UseColors(true);
	ShowTraces(false);

	time_t raw = time(NULL);
	char sTime[25];

	strftime(sTime, 25, "%m/%d/%Y @ %H:%M:%S", localtime(&raw));

	Trace("------------------------------------------------------------");
	Trace("-- Rhythm-Station log started at %s", sTime);
	Trace("------------------------------------------------------------");
}

Logger::~Logger()
{
	ShowTraces(false);

	Trace("------------------------------------------------------------");
	Trace("-- Log finished --");
	Trace("------------------------------------------------------------");

	// close & flush
	PHYSFS_flush(m_File);
	PHYSFS_close(m_File);
}

void GetTime(char *sTime)
{
	time_t raw = time(NULL);

	// Format the time (mm/dd hh:mm:ss) i.e. 12/30 06:24:50
	strftime(sTime, 25, "[%H:%M:%S]", localtime(&raw));
}

void Logger::Internal(string in, string message, ConsoleColor color, bool show)
{
	ostringstream stream;
	string str;
	char sTime[25];

	GetTime(sTime);

	stream << sTime << " ";
	if (message.length() > 1)
		stream << message << ": ";
	stream << in << "\n";
	str = stream.str();

	// Write out buffer to file
	PHYSFS_write(m_File, str.c_str(), str.length(), 1);

	// Write to console
	if (!show)
		return;

	if (!m_use_colors)
	{
		printf("%s", str.c_str());
		return;
	}

	// Rebuild message with colors
	stream.str("");
	stream << sTime << " ";

	if (color != FG_INVALID)
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

	Internal(str, "Debug", FG_CYAN);
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

/**
 * Colby Klein, Jarno Purontakanen, Matt Vandermeulen (c) 2011
 * Licensed under the terms of the MIT license. See License.txt.
 */
