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
//	System interface for sound.
//
//-----------------------------------------------------------------------------

#include "doomtype.h"
static const d_char rcsid[] = "$Id: i_unix.c,v 1.5 1997/02/03 22:45:10 b1 Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <math.h>

#include "z_zone.h"

#include "i_system.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"

#include "doomdef.h"

// The number of internal mixing channels,
//  the samples calculated for each mixing step,
//  the size of the 16bit, 2 hardware channel (stereo)
//  mixing buffer, and the samplerate of the raw data.


// Needed for calling the actual sound output.
#define SAMPLECOUNT		512
#define NUM_CHANNELS		8
// It is 2 for 16bit, and 2 for two channels.
#define BUFMUL                  4
#define MIXBUFFERSIZE		(SAMPLECOUNT*BUFMUL)

#define SAMPLERATE		11025	// Hz
#define SAMPLESIZE		2   	// 16bit

// The actual lengths of all sound effects.
d_int lengths[NUMSFX];

// The global mixing buffer.
// Basically, samples from all active internal channels
//  are modifed and added, and stored in the buffer
//  that is submitted to the audio device.
d_short mixbuffer[MIXBUFFERSIZE];


// The channel step amount...
d_uint channelstep[NUM_CHANNELS];
// ... and a 0.16 bit remainder of last step.
d_uint channelstepremainder[NUM_CHANNELS];


// The channel data pointers, start and end.
byte*	channels[NUM_CHANNELS];
byte*	channelsend[NUM_CHANNELS];


// Time/gametic that the channel started playing,
//  used to determine oldest, which automatically
//  has lowest priority.
// In case number of active sounds exceeds
//  available channels.
d_uint		channelstart[NUM_CHANNELS];

// The sound in channel handles,
//  determined on registration,
//  might be used to unregister/stop/modify,
//  currently unused.
d_uint 		channelhandles[NUM_CHANNELS];

// SFX id of the playing sound effect.
// Used to catch duplicates (like chainsaw).
d_uint		channelids[NUM_CHANNELS];

// Pitch to stepping lookup, unused.
d_uint		steptable[256];

// Volume lookups.
d_uint		vol_lookup[128*256];

// Hardware left and right channel volume lookup.
d_uint*		channelleftvol_lookup[NUM_CHANNELS];
d_uint*		channelrightvol_lookup[NUM_CHANNELS];

//
// SFX API
// Note: this was called by S_Init.
// However, whatever they did in the
// old DPMS based DOS version, this
// were simply dummies in the Linux
// version.
// See soundserver initdata().
//
void I_SetChannels()
{
}	

 
void I_SetSfxVolume(d_uint volume)
{
  // Identical to DOS.
  // Basically, this should propagate
  //  the menu/config file setting
  //  to the state variable used in
  //  the mixing.
  snd_SfxVolume = volume;
}

// MUSIC API - dummy. Some code from DOS version.
void I_SetMusicVolume(d_uint volume)
{
  // Internal state variable.
  snd_MusicVolume = volume;
  // Now set volume on output device.
  // Whatever( snd_MusciVolume );
}


//
// Retrieve the raw data lump index
//  for a given SFX name.
//
d_int I_GetSfxLumpNum(sfxinfo_t* sfx)
{
    char namebuf[9];
    sprintf(namebuf, "ds%s", sfx->name);
    return W_GetNumForName(namebuf);
}

//
// Starting a sound means adding it
//  to the current list of active sounds
//  in the internal channels.
// As the SFX info struct contains
//  e.g. a pointer to the raw data,
//  it is ignored.
// As our sound handling does not handle
//  priority, it is ignored.
// Pitching (that is, increased speed of playback)
//  is set, but currently not used by mixing.
//
d_int
I_StartSound
( d_int		id,
  d_int		vol,
  d_int		sep,
  d_int		pitch,
  d_int		priority )
{
}



void I_StopSound (d_int handle)
{
  // You need the handle returned by StartSound.
  // Would be looping all channels,
  //  tracking down the handle,
  //  an setting the channel to zero.
}


d_int I_SoundIsPlaying(d_int handle)
{
  return 0;
    // Ouch.
    return gametic < handle;
}

//
// This function loops all active (internal) sound
//  channels, retrieves a given number of samples
//  from the raw sound data, modifies it according
//  to the current (internal) channel parameters,
//  mixes the per channel samples into the global
//  mixbuffer, clamping it to the allowed range,
//  and sets up everything for transferring the
//  contents of the mixbuffer to the (two)
//  hardware channels (left and right, that is).
//
// This function currently supports only 16bit.
//
void I_UpdateSound( void )
{
}

// 
// This would be used to write out the mixbuffer
//  during each game loop update.
// Updates sound buffer and audio device at runtime. 
// It is called during Timer interrupt with SNDINTR.
// Mixing now done synchronous, and
//  only output be done asynchronous?
//
void I_SubmitSound(void)
{
}

void I_UpdateSoundParams( d_int handle, d_int vol, d_int sep, d_int pitch )
{
}

void I_ShutdownSound(void)
{    
}

void I_InitSound()
{
}

//
// MUSIC API.
// Still no music done.
// Remains. Dummies.
//
void I_InitMusic(void)
{
}

void I_ShutdownMusic(void)
{
}

void I_PlaySong(d_int handle, d_int looping)
{
}

void I_PauseSong (d_int handle)
{
}

void I_ResumeSong (d_int handle)
{
}

void I_StopSong(d_int handle)
{
}

void I_UnRegisterSong(d_int handle)
{
}

d_int I_RegisterSong(void* data)
{
  return 1;
}

// Is the song playing?
d_int I_QrySongPlaying(d_int handle)
{
  return false;
}


// Interrupt handler.
void I_HandleSoundTimer( d_int ignore )
{
  // TODO Add sound.
  // UNUSED, but required.
  ignore = 0;
  return;
}

// Get the interrupt. Set duration in millisecs.
d_int I_SoundSetTimer( d_int duration_of_tick )
{
  // TODO Add sound.
}


// Remove the interrupt. Set duration to zero.
void I_SoundDelTimer()
{
  // TODO Add sound.
}
