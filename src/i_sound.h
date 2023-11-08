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
//
// DESCRIPTION:
//	System interface, sound.
//
//-----------------------------------------------------------------------------

#ifndef __I_SOUND__
#define __I_SOUND__

#include "doomdef.h"

#include "doomstat.h"
#include "sounds.h"



// Init at program start...
void I_InitSound();

// ... update sound buffer and audio device at runtime...
void I_UpdateSound(void);
void I_SubmitSound(void);

// ... shut down and relase at program termination.
void I_ShutdownSound(void);


//
//  SFX I/O
//

// Initialize channels?
void I_SetChannels();

// Get raw data lump index for sound descriptor.
d_int I_GetSfxLumpNum (sfxinfo_t* sfxinfo );

// Starts a sound in a particular sound channel.
d_int I_StartSound ( d_int id, d_int vol, d_int sep, d_int pitch, d_int priority );

// Stops a sound channel.
void I_StopSound(d_int handle);

// Called by S_*() functions
//  to see if a channel is still playing.
// Returns 0 if no longer playing, 1 if playing.
d_int I_SoundIsPlaying(d_int handle);

// Updates the volume, separation,
//  and pitch of a sound channel.
void
I_UpdateSoundParams
( d_int		handle,
  d_int		vol,
  d_int		sep,
  d_int		pitch );


//
//  MUSIC I/O
//
void I_InitMusic(void);
void I_ShutdownMusic(void);
// Volume.
void I_SetMusicVolume(d_uint volume);
// PAUSE game handling.
void I_PauseSong(d_int handle);
void I_ResumeSong(d_int handle);
// Registers a song handle to song data.
d_int I_RegisterSong(void *data);
// Called by anything that wishes to start music.
//  plays a song, and when the song is done,
//  starts playing it again in an endless loop.
// Horrible thing to do, considering.
void
I_PlaySong
( d_int		handle,
  d_int		looping );
// Stops a song over 3 seconds.
void I_StopSong(d_int handle);
// See above (register), then think backwards
void I_UnRegisterSong(d_int handle);



#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
