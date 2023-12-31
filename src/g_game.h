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
//   Duh.
// 
//-----------------------------------------------------------------------------


#ifndef __G_GAME__
#define __G_GAME__

#include "doomdef.h"
#include "d_event.h"



//
// GAME
//
void G_DeathMatchSpawnPlayer (d_int playernum);

void G_InitNew (skill_t skill, d_int episode, d_int map);

// Can be called by the startup code or M_Responder.
// A normal game starts at map 1,
// but a warp test can start elsewhere
void G_DeferedInitNew (skill_t skill, d_int episode, d_int map);

void G_DeferedPlayDemo (d_char* demo);

// Can be called by the startup code or M_Responder,
// calls P_SetupLevel or W_EnterWorld.
void G_LoadGame (d_char* name);

void G_DoLoadGame (void);

// Called by M_Responder.
void G_SaveGame (d_int slot, d_char* description);

// Only called by startup code.
void G_RecordDemo (d_char* name);

void G_BeginRecording (void);

void G_PlayDemo (d_char* name);
void G_TimeDemo (d_char* name);
boolean G_CheckDemoStatus (void);

void G_ExitLevel (void);
void G_SecretExitLevel (void);

void G_WorldDone (void);

void G_Ticker (void);
boolean G_Responder (event_t*	ev);

void G_ScreenShot (void);


#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
