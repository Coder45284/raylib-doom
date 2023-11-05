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
//	Mission start screen wipe/melt, special effects.
//	
//-----------------------------------------------------------------------------


#ifndef __F_WIPE_H__
#define __F_WIPE_H__

//
//                       SCREEN WIPE PACKAGE
//

enum
{
    // simple gradual pixel change for 8-bit only
    wipe_ColorXForm,
    
    // weird screen melt
    wipe_Melt,	

    wipe_NUMWIPES
};

d_int wipe_StartScreen(
  d_int x,
  d_int y,
  d_int width,
  d_int height );


d_int
wipe_EndScreen
( d_int		x,
  d_int		y,
  d_int		width,
  d_int		height );


d_int
wipe_ScreenWipe
( d_int		wipeno,
  d_int		x,
  d_int		y,
  d_int		width,
  d_int		height,
  d_int		ticks );

#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
