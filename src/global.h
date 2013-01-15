#pragma once
//#error not_even_once

// Some branch prediction hints for GCC.
#ifdef __GNUC__
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)
#else
#define likely(x)	x
#define unlikely(x)	x
#endif

// Helps prevent heap corruption.
#define SAFE_DELETE(p) if( (p) != NULL ) delete (p); (p) = NULL;
