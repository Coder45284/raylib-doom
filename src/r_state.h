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
//	Refresh/render internal state variables (global).
//
//-----------------------------------------------------------------------------


#ifndef __R_STATE__
#define __R_STATE__

// Need data structure definitions.
#include "d_player.h"
#include "r_data.h"

//
// Refresh internal data structures,
//  for rendering.
//

// needed for texture pegging
extern fixed_t*		textureheight;

// needed for pre rendering (fracs)
extern fixed_t*		spritewidth;

extern fixed_t*		spriteoffset;
extern fixed_t*		spritetopoffset;

extern lighttable_t*	colormaps;

extern d_int		viewwidth;
extern d_int		scaledviewwidth;
extern d_int		viewheight;

extern d_int		firstflat;

// for global animation
extern d_int*		flattranslation;
extern d_int*		texturetranslation;


// Sprite....
extern d_int		firstspritelump;
extern d_int		lastspritelump;
extern d_int		numspritelumps;



//
// Lookup tables for map data.
//
extern d_int		numsprites;
extern spritedef_t*	sprites;

extern d_int		numvertexes;
extern vertex_t*	vertexes;

extern d_int		numsegs;
extern seg_t*		segs;

extern d_int		numsectors;
extern sector_t*	sectors;

extern d_int		numsubsectors;
extern subsector_t*	subsectors;

extern d_int		numnodes;
extern node_t*		nodes;

extern d_int		numlines;
extern line_t*		lines;

extern d_int		numsides;
extern side_t*		sides;


//
// POV data.
//
extern fixed_t		viewx;
extern fixed_t		viewy;
extern fixed_t		viewz;

extern angle_t		viewangle;
extern player_t*	viewplayer;


// ?
extern angle_t		clipangle;

extern d_int		viewangletox[FINEANGLES/2];
extern angle_t		xtoviewangle[SCREENWIDTH+1];
//extern fixed_t		finetangent[FINEANGLES/2];

extern fixed_t		rw_distance;
extern angle_t		rw_normalangle;



// angle to line origin
extern d_int		rw_angle1;

// Segs count?
extern d_int		sscount;

extern visplane_t*	floorplane;
extern visplane_t*	ceilingplane;


#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
