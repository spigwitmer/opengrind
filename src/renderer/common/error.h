#pragma once

// Don't include GL headers in this file so that GL prior to 3 isn't an issue.

#include "global.h"

#ifdef DEBUG

// Very important not to let any errors like this slip!
#define CheckError()\
{\
	GLenum err = glGetError();\
	if (unlikely(err != GL_NO_ERROR))\
	{\
		for (; err != GL_NO_ERROR; err = glGetError())\
		{\
			switch (err)\
			{\
				case GL_INVALID_ENUM: LOG->Error("GL_INVALID_ENUM"); break;\
				case GL_INVALID_OPERATION: LOG->Error("GL_INVALID_OPERATION"); break;\
				case GL_INVALID_VALUE: LOG->Error("GL_INVALID_VALUE"); break;\
				case GL_OUT_OF_MEMORY: LOG->Error("GL_OUT_OF_MEMORY"); break;\
				default: break;\
			}\
		}\
	}\
}
#else

// Errors don't need to be so nasty in a release build, but still warn.	
#define CheckError()\
{\
	GLenum err = glGetError();\
	if (unlikely(err != GL_NO_ERROR))\
	{\
		for (; err != GL_NO_ERROR; err = glGetError())\
		{\
			switch (err)\
			{\
				case GL_INVALID_ENUM: LOG->Warn("GL_INVALID_ENUM"); break;\
				case GL_INVALID_OPERATION: LOG->Warn("GL_INVALID_OPERATION"); break;\
				case GL_INVALID_VALUE: LOG->Warn("GL_INVALID_VALUE"); break;\
				case GL_OUT_OF_MEMORY: LOG->Error("GL_OUT_OF_MEMORY"); break;\
				default: break;\
			}\
		}\
	}\
}
#endif
