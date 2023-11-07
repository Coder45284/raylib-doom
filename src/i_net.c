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
//     This is a blank implementation of i_int. It does not do any networking
//     at all.
//
//-----------------------------------------------------------------------------

#include "doomtype.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "i_system.h"
#include "d_event.h"
#include "d_net.h"
#include "m_argv.h"

#include "doomstat.h"

#include "i_net.h"

//
// NETWORKING
//

//
// I_InitNetwork
//
void I_InitNetwork (void)
{
    // TODO Maybe add multiplayer
    static doomcom_t doomcom_memory;

    // Multiplayer not supported.
    doomcom = &doomcom_memory;
    memset (doomcom, 0, sizeof(*doomcom) );

    // single player game
    netgame = false;
    doomcom->id = DOOMCOM_ID;
    doomcom-> ticdup = 1;
    doomcom->numplayers = doomcom->numnodes = 1;
    doomcom->deathmatch = false;
    doomcom->consoleplayer = 0;
}


void I_NetCmd (void)
{
    // TODO Maybe add this.
}

