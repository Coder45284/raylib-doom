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
//	WAD I/O functions.
//
//-----------------------------------------------------------------------------


#ifndef __W_WAD__
#define __W_WAD__

#include "doomtype.h"

#ifdef __GNUG__
#pragma interface
#endif


//
// TYPES
//
typedef struct
{
    // Should be "IWAD" or "PWAD".
    d_char		identification[4];
    d_int		numlumps;
    d_int		infotableofs;
    
} wadinfo_t;


typedef struct
{
    d_int		filepos;
    d_int		size;
    d_char		name[8];
    
} filelump_t;

//
// WADFILE I/O related stuff.
//
typedef struct
{
    d_char	name[8];
    d_int	handle;
    d_int	position;
    d_int	size;
} lumpinfo_t;


extern	void**      lumpcache;
extern	lumpinfo_t* lumpinfo;
extern	d_int       numlumps;

void  W_InitMultipleFiles (d_char** filenames);
void  W_Reload (void);

d_int W_CheckNumForName (d_char* name);
d_int W_GetNumForName (d_char* name);

d_int W_LumpLength (d_int lump);
void  W_ReadLump (d_int lump, void *dest);

void* W_CacheLumpNum (d_int lump, d_int tag);
void* W_CacheLumpName (d_char* name, d_int tag);




#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
