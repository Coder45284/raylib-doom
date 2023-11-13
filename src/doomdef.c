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
// $Log:$
//
// DESCRIPTION:
//  DoomDef - basic defines for DOOM, e.g. Version, game mode
//   and skill level, and display parameters.
//
//-----------------------------------------------------------------------------

#include "doomtype.h"

#include "doomdef.h"
#include <ctype.h>

const char GAMEMODES[ 5 ][ 13 ] = { {"SHAREWARE"}, {"REGISTERED"}, {"COMMERCIAL"}, {"RETAIL"}, {"INDETERMINED"} };

boolean D_StrCaseEqual( const d_char *const left, const d_char *const right ) {
    size_t length_left = strlen( left );

    if( length_left != strlen( right ) )
        return false;

    return D_StrNCaseEqual( left, right, length_left );
}

boolean D_StrNCaseEqual( const d_char *const left, const d_char *const right, size_t n ) {
    for( size_t i = 0; left[i] != '\0' && i < n; i++)
        if( toupper(left[i]) != toupper(right[i]) )
            return false;

    return true;
}
