// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//	Simple basic typedefs, isolated here to make it easier
//	 separating modules.
//    
//-----------------------------------------------------------------------------


#ifndef __DOOMTYPE__
#define __DOOMTYPE__

#include <stdint.h>

#ifndef __BYTEBOOL__
#define __BYTEBOOL__
// Fixed to use builtin bool type with C++.
#ifdef __cplusplus
typedef bool boolean;
#else
typedef enum {false, true} boolean;
#endif
typedef uint8_t byte;
#endif

typedef char     d_char;
typedef int16_t  d_short;
typedef uint16_t d_ushort;
typedef int32_t  d_int;
typedef uint32_t d_uint;
typedef int32_t  d_long;
typedef uint32_t  d_ulong;

typedef intptr_t   d_intptr;
typedef uintptr_t  d_uintptr;

#define D_MAX_CHAR  ((d_char)0x7f)
#define D_MAX_SHORT ((d_short)0x7fff)

// Max pos 32-bit int.
#define D_MAX_INT   ((d_int)0x7fffffff)
#define D_MAX_LONG  ((d_long)0x7fffffff)

#define D_MIN_CHAR  ((d_char)0x80)
#define D_MIN_SHORT ((d_short)0x8000)
// Max negative 32-bit integer.
#define D_MIN_INT   ((d_int) 0x80000000)
#define D_MIN_LONG  ((d_long)0x80000000)

#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
