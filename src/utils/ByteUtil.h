/* Defines some basic byte operations for stdint types. */
#pragma once

#include <stdint.h>

// these aren't in a namespace because they're meant to be inline calls.
// consequently, they need to be declared entirely in the header. */

/*
 * function declarations
 */
uint8_t LowByte(const uint16_t &i);
uint8_t HighByte(const uint16_t &i);

uint16_t LowWord(const uint32_t &i);
uint16_t HighWord(const uint32_t &i);

uint32_t LowDWord(const uint64_t &i);
uint32_t HighDWord(const uint64_t &i);

// swaps between little-endian and big-endian
uint16_t ByteSwap16(const uint16_t &i);
uint32_t ByteSwap24(const uint32_t &i);
uint32_t ByteSwap32(const uint32_t &i);
uint64_t ByteSwap64(const uint64_t &i);

// swaps the halves of each type (needed for a few operations)
uint32_t WordSwap(const uint32_t &i);
uint64_t DWordSwap(const uint64_t &i);

// converts two 8-bit values into one 16-bit value
uint16_t Make16(const uint8_t &a, const uint8_t &b);
uint16_t Make16(const uint8_t *vals);

// converts three 8-bit values into one 24-bit value, returned as a 32-bit value
uint32_t Make24(const uint8_t &a, const uint8_t &b, const uint8_t &c);
uint32_t Make24(const uint8_t *vals);

// converts four 8-bit values into one 32-bit value
uint32_t Make32(const uint8_t &a, const uint8_t &b, const uint8_t &c, const uint8_t &d);
uint32_t Make32(const uint8_t *vals);

// converts eight 8-bit values into one 64-bit value
uint64_t Make64(const uint8_t &a, const uint8_t &b, const uint8_t &c, const uint8_t &d, const uint8_t &e, const uint8_t &f, const uint8_t &g, const uint8_t &h);
uint64_t Make64(const uint8_t *vals);

// converts two 16-bit values into one 32-bit value
uint32_t Make32(const uint16_t &a, const uint16_t &b);
uint32_t Make32(const uint16_t *vals);

// splits one 16-bit values into two 8-bit values
void Split16(const uint16_t &i, uint8_t &a, uint8_t &b);
void Split16(const uint16_t &i, uint8_t *vals);

// splits one 24-bit value into three 8-bit values
void Split24(const uint32_t &i, uint8_t &a, uint8_t &b, uint8_t &c);
void Split24(const uint32_t &i, uint8_t *vals);

// splits one 32-bit value into four 8-bit values
void Split32(const uint32_t &i, uint8_t &a, uint8_t &b, uint8_t &c, uint8_t &d);
void Split32(const uint32_t &i, uint8_t *vals);

// splits one 32-bit value into two 16-bit values
void Split32(const uint32_t &i, uint16_t &a, uint16_t &b);
void Split32(const uint32_t &i, uint16_t *vals);

// splits one 64-bit value into eight 8-bit values
void Split64(const uint64_t &i, uint8_t &a, uint8_t &b, uint8_t &c, uint8_t &d, uint8_t &e, uint8_t &f, uint8_t &g, uint8_t &h);
void Split64(const uint64_t &i, uint8_t *vals);

// splits one 64-bit value into four 16-bit values
void Split64(const uint64_t &i, uint16_t *vals);

// splits one 64-bit value into two 32-bit values
void Split64(const uint64_t &i, uint32_t &a, uint32_t &b);
void Split64(const uint64_t &i, uint32_t *vals);

/*
 * function definitions
 */

inline uint8_t LowByte(const uint16_t &i)		{ return uint8_t(i); }
inline uint8_t HighByte(const uint16_t &i)		{ return uint8_t(i>>8); }

inline uint16_t LowWord(const uint32_t &i)		{ return uint16_t(i); }
inline uint16_t HighWord(const uint32_t &i)		{ return uint16_t(i>>16); }

inline uint32_t LowDWord(const uint64_t &i)		{ return uint32_t(i);	}
inline uint32_t HighDWord(const uint64_t &i)	{ return uint32_t(i>>32); }

inline uint16_t ByteSwap16(const uint16_t &i)
{
	return (i << 8) | (i >> 8);
}

inline uint32_t ByteSwap24(const uint32_t &i)
{
	return ByteSwap32(i) >> 8;
}

inline uint32_t ByteSwap32(const uint32_t &i)
{
	return (i >> 24) | (i << 24) | ((i >> 8) & 0x00FF00) | ((i << 8) & 0xFF0000);
}

inline uint64_t ByteSwap64(const uint64_t &i)
{
	return uint64_t(ByteSwap32(LowDWord(i))) << 32 | ByteSwap32(HighDWord(i));
}

inline uint32_t WordSwap(const uint32_t &i)
{
	return LowWord(i) << 16 | HighWord(i);
}

inline uint64_t DWordSwap(const uint64_t &i)
{
	return uint64_t(LowDWord(i)) << 32 | HighDWord(i);
}

inline uint16_t Make16(const uint8_t &a, const uint8_t &b)
{
	return (a << 8) | b;
}

inline uint16_t Make16(const uint8_t *vals)
{
	return Make16(vals[0], vals[1]);
}

inline uint32_t Make24(const uint8_t &a, const uint8_t &b, const uint8_t &c)
{
	return (a << 16) | (b << 8) | c;
}

inline uint32_t Make24(const uint8_t *vals)
{
	return Make24(vals[0], vals[1], vals[2]);
}

inline uint32_t Make32(const uint8_t &a, const uint8_t &b, const uint8_t &c, const uint8_t &d)
{
	return (a << 24) | (b << 16) | (c << 8) | d;
}

inline uint32_t Make32(const uint8_t *vals)
{
	return Make32(vals[0], vals[1], vals[2], vals[3]);
}

inline uint32_t Make32(const uint16_t &a, const uint16_t &b)
{
	return (a << 16) | b;
}

inline uint32_t Make32(const uint16_t *vals)
{
	return Make32(vals[0], vals[1]);
}

// this is a bit slower, but avoids shifting problems.
inline uint64_t Make64( const uint8_t &a, const uint8_t &b, const uint8_t &c, const uint8_t &d, const uint8_t &e, const uint8_t &f, const uint8_t &g, const uint8_t &h )
{
	return uint64_t(Make32(a,b,c,d)) << 32 | Make32(e,f,g,h);
}

inline uint64_t Make64( const uint8_t *vals )
{
	return Make64( vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6], vals[7] );
}

inline void Split16( const uint16_t &i, uint8_t &a, uint8_t &b )
{
	a = HighByte(i);
	b = LowByte(i);
}

inline void Split16( const uint16_t &i, uint8_t *vals )
{
	Split16( i, vals[0], vals[1] );
}

inline void Split24( const uint32_t &i, uint8_t &a, uint8_t &b, uint8_t &c )
{
	a = i >> 16;
	b = HighByte( i );
	c = LowByte( i );
}

inline void Split24( const uint32_t &i, uint8_t *vals )
{
	Split24( i, vals[0], vals[1], vals[2] );
}

/* splits a 32-bit value into two 16-bit values */
inline void Split32( const uint32_t &i, uint16_t &a, uint16_t &b )
{
	a = HighWord(i);
	b = LowWord(i);
}

inline void Split32( const uint32_t &i, uint16_t *vals )
{
	Split32( i, vals[0], vals[1] );
}

inline void Split32( const uint32_t &i, uint8_t &a, uint8_t &b, uint8_t &c, uint8_t &d )
{
	a = uint8_t(i>>24);
	b = uint8_t(i>>16);
	c = uint8_t(i>>8);
	d = uint8_t(i);
}

inline void Split32( const uint32_t &i, uint8_t *vals )
{
	Split32( i, vals[0], vals[1], vals[2], vals[3] );
}

inline void Split64( const uint64_t &i, uint8_t &a, uint8_t &b, uint8_t &c, uint8_t &d, uint8_t &e, uint8_t &f, uint8_t &g, uint8_t &h )
{
	a = uint8_t(i>>56);
	b = uint8_t(i>>48);
	c = uint8_t(i>>40);
	d = uint8_t(i>>32);
	e = uint8_t(i>>24);
	f = uint8_t(i>>16);
	g = uint8_t(i>>8);
	h = uint8_t(i);
}

inline void Split64( const uint64_t &i, uint8_t *vals )
{
	Split32( HighDWord(i), &vals[0] );
	Split32( LowDWord(i), &vals[4] );
}

inline void Split64( const uint64_t &i, uint32_t &a, uint32_t &b )
{
	a = HighDWord(i);
	b = LowDWord(i);
}

inline void Split64( const uint64_t &i, uint32_t *vals )
{
	Split64( i, vals[0], vals[1] );
}
