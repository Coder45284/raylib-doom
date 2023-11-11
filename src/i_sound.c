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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <math.h>

#include "z_zone.h"

#include "i_system.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "m_swap.h"
#include "w_wad.h"

#include "doomdef.h"

#include "raylib.h"

#define TSF_IMPLEMENTATION
#include "tsf.h"

#define SFX_CHANNELS          1
#define SFX_BYTES_PER_CHANNEL 1
#define SFX_BITS_PER_CHANNEL  SFX_BYTES_PER_CHANNEL * 8
#define SFX_SAMPLE_RATE       11025 // Hz
#define SFX_PADDED_BYTES      16

Sound sfx_audios[NUMSFX];
d_uint sfx_audio_amount = 0;
Sound* sfx_audio_table[NUMSFX];

void* getsfx( d_char* sfxname, Sound* sfxsound ) {
    byte*  sfx;
    d_int  sfxlump;
    d_int  size;
    d_char name[20];
    Wave   wave;

    snprintf(name, 20, "ds%s", sfxname);

    if ( W_CheckNumForName(name) == -1 )
        sfxlump = W_GetNumForName("dspistol");
    else
        sfxlump = W_GetNumForName(name);

    size = W_LumpLength( sfxlump );

    sfx = (byte*)W_CacheLumpNum( sfxlump, PU_STATIC );

    d_ushort header_number  = SHORT( *(d_ushort*)(sfx + 0) );
    d_ushort sample_rate_hz = SHORT( *(d_ushort*)(sfx + 1 * sizeof(d_short)) );
    d_ulong  padded_samples = LONG(  *(d_ulong*)( sfx + 2 * sizeof(d_short)) );

    wave.frameCount = (padded_samples - 2 * SFX_PADDED_BYTES) / (SFX_BYTES_PER_CHANNEL * SFX_CHANNELS);
    wave.sampleRate = sample_rate_hz;
    wave.sampleSize = SFX_BITS_PER_CHANNEL;
    wave.channels   = SFX_CHANNELS;
    wave.data       = sfx + 2 * sizeof(d_short) + sizeof(d_ulong) + SFX_PADDED_BYTES;

    *sfxsound = LoadSoundFromWave( wave );

    return sfx;
}

void I_SetChannels()
{
}

 
void I_SetSfxVolume(d_uint volume)
{
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

    snprintf(namebuf, 9, "ds%s", sfx->name);

    return W_GetNumForName(namebuf);
}

d_int I_StartSound (d_int id, d_int vol, d_int sep, d_int pitch, d_int priority)
{
    PlaySound( *sfx_audio_table[id] );
    I_UpdateSoundParams( id, vol, sep, pitch );
    return id;
}



void I_StopSound (d_int handle)
{
    StopSound( *sfx_audio_table[handle] );
}


d_int I_SoundIsPlaying(d_int handle)
{
    return IsSoundPlaying( *sfx_audio_table[handle] );
}

void I_UpdateSound( void )
{
}

void I_SubmitSound(void)
{
}

#define PITCH_LEVEL_FUNCTION(min_pitch, pitch) (2.0 / 255.0) * (1.0 - min_pitch) * pitch + min_pitch

void I_UpdateSoundParams(d_int id, d_int vol, d_int sep, d_int pitch)
{
    SetSoundVolume( *sfx_audio_table[id], vol * (1.0/15.0) );
    SetSoundPitch( *sfx_audio_table[id], PITCH_LEVEL_FUNCTION( 0.5, pitch ) );
    SetSoundPan( *sfx_audio_table[id], 1.0 - sep * (1.0/255.0));
}

void I_ShutdownSound(void)
{
    // Clean up all the audio data.
    for( d_int i = 0; i < sfx_audio_amount; i++ )
        UnloadSound( sfx_audios[i] );
    sfx_audio_amount = 0;

    if( IsAudioDeviceReady() )
        CloseAudioDevice();
}

void I_InitSound()
{
    InitAudioDevice();

    // Initialize external data (all sounds) at start, keep static.
    fprintf( stderr, "I_InitSound: ");

    for( d_int i = 1; i < NUMSFX; i++ )
    {
        // Alias? Example is the chaingun sound linked to pistol.
        if (!S_sfx[i].link)
        {
            // Load data from WAD file.
            S_sfx[i].data = getsfx( S_sfx[i].name, &sfx_audios[sfx_audio_amount] );
            sfx_audio_table[i] = &sfx_audios[sfx_audio_amount];
            sfx_audio_amount++;
        }
        else
        {
            // Previously loaded already?
            S_sfx[i].data = S_sfx[i].link->data;
            sfx_audio_table[i] = &sfx_audios[(S_sfx[i].link - S_sfx)/sizeof(sfxinfo_t)];
        }
    }
}

typedef struct MusicBufferStruct {
    void *mus_data;
    size_t mus_data_length; // bytes.

    void *mus_events;
    size_t mus_events_limit; // bytes.

    size_t mus_event_head;
    d_int looping;
    d_int paused;
    d_int reset;
} MusicBuffer;

#define MAX_MUSIC_BUFFERS 4
static MusicBuffer music_buffers[MAX_MUSIC_BUFFERS];
static tsf* sound_font = NULL;
static d_int current_mus_handle = -1;
static AudioStream mus_stream;

#define MUS_TABLE_LIMIT 15
#define MUS_DELAY_RATE  140

#define MIDI_SAMPLE_RATE 44100
#define MIDI_CHANNELS    2
#define MIDI_BUFFER_TIME 4 // Seconds
#define MIDI_BUFFER_SIZE (MIDI_BUFFER_TIME * MIDI_CHANNELS * sizeof(short) * MIDI_SAMPLE_RATE)

#if MIDI_CHANNELS == 1
#define MIDI_TSF_OPTIONS TSF_MONO
#elif MIDI_CHANNELS == 2
#define MIDI_TSF_OPTIONS TSF_STEREO_INTERLEAVED
#else
#error "Unsupported MIDI type"
#endif

void MidiInputCallback(void* buffer, unsigned int frames )
{
    /*if( current_mus_handle >= MAX_MUSIC_BUFFERS ||
        current_mus_handle < 0 ||
        music_buffers[ current_mus_handle ].mus_data == NULL ||
        music_buffers[ current_mus_handle ].mus_data_length == 0 )
    {
        memset( buffer, 0, frames * MIDI_CHANNELS * sizeof(short) );
        return;
    }*/

    short *values = (short*)buffer;

    for( unsigned int f = 0; f < frames; f++ ) {
        for( unsigned int c = 0; c < MIDI_CHANNELS; c++ ) {
            values[MIDI_CHANNELS * f + c] = f % 8192;
        }
    }
}

//
// MUSIC API.
//
void I_InitMusic(void)
{
    for( int i = 0; i < MAX_MUSIC_BUFFERS; i++ ) {
        music_buffers[i].mus_data = NULL;
        music_buffers[i].mus_data_length = 0;
    }

    d_char soundfont_filepath[] = "FluidR3_GM.sf2";

    sound_font = tsf_load_filename(soundfont_filepath);

    if( sound_font == NULL )
        printf( "Soundfont \"%s\" not found! No music playback!\n", soundfont_filepath);

    tsf_set_output(sound_font, MIDI_TSF_OPTIONS, MIDI_SAMPLE_RATE, 0);

    SetAudioStreamBufferSizeDefault(MIDI_BUFFER_SIZE);

    mus_stream = LoadAudioStream( MIDI_SAMPLE_RATE, 16, MIDI_CHANNELS );

    SetAudioStreamCallback( mus_stream, MidiInputCallback );

    PlayAudioStream( mus_stream );
}

void I_ShutdownMusic(void)
{
    if( sound_font != NULL )
        tsf_close( sound_font );

    sound_font = NULL;
}

void I_PlaySong(d_int handle, d_int looping)
{
    if( handle >= MAX_MUSIC_BUFFERS || handle < 0 )
        return;

    music_buffers[handle].mus_event_head = 0;
    music_buffers[handle].looping = looping;
    music_buffers[handle].paused = false;
    music_buffers[handle].reset = true;
}

void I_PauseSong (d_int handle)
{
    if( handle >= MAX_MUSIC_BUFFERS || handle < 0 )
        return;

    music_buffers[handle].paused = true;
}

void I_ResumeSong (d_int handle)
{
    if( handle >= MAX_MUSIC_BUFFERS || handle < 0 )
        return;

    music_buffers[handle].paused = false;
}

void I_StopSong(d_int handle)
{
    if( handle >= MAX_MUSIC_BUFFERS || handle < 0 )
        return;

    music_buffers[handle].reset = true;
}

void I_UnRegisterSong(d_int handle)
{
    if( handle >= MAX_MUSIC_BUFFERS || handle < 0 )
        return;

    music_buffers[ handle ].mus_data = NULL;
    music_buffers[ handle ].mus_data_length = 0;
}

d_int I_RegisterSong(void* data)
{
    static byte mus_to_midi[MUS_TABLE_LIMIT] = {
          0, //  0
          0, //  1
          1, //  2
          7, //  3
         10, //  4
         11, //  5
         91, //  6
         93, //  7
         64, //  8
         67, //  9
        120, // 10
        123, // 11
        126, // 12
        127, // 13
        121, // 14
    };

    void* data_head = data;
    d_ushort song_length = 0;
    d_ushort song_offset = 0;
    d_ushort primary_channel_amount = 0;
    d_ushort second_channel_amount = 0;
    d_ushort instrument_amount = 0;
    byte channel_last_volumes[16];
    byte magic[4];

    // Initialize volumes.
    for( int i = 0; i < 16; i++ ) {
        channel_last_volumes[i] = 0;
    }

    magic[0] = *(byte*)(data_head++);
    magic[1] = *(byte*)(data_head++);
    magic[2] = *(byte*)(data_head++);
    magic[3] = *(byte*)(data_head++);

    song_length = SHORT( *(d_ushort*)data_head );
    data_head += sizeof(d_ushort);
    song_offset = SHORT( *(d_ushort*)data_head );
    data_head += sizeof(d_ushort);
    primary_channel_amount = SHORT( *(d_ushort*)data_head );
    data_head += sizeof(d_ushort);
    second_channel_amount = SHORT( *(d_ushort*)data_head );
    data_head += sizeof(d_ushort);
    instrument_amount = SHORT( *(d_ushort*)data_head );
    data_head += sizeof(d_ushort);

    printf( "I_RegisterSong\n"
        "   Magic %c%c%c, 0x%x,\n"
        "   Length bytes %d,\n"
        "   Offset 0x%x,\n"
        "   Primary Channel Amount %d,\n"
        "   Secondary Channel Amount %d,\n"
        "   Instrument Amount %d\n",
        magic[0], magic[1], magic[2], magic[3], song_length, song_offset,
        primary_channel_amount, second_channel_amount, instrument_amount );

    // Event variables.
    d_short midi_event      = 0;
    d_short midi_channel    = 0;
    byte    midi_parameter1 = 0;
    byte    midi_parameter2 = 0;

    short *midi_buffer = malloc( 1 );
    size_t midi_buffer_length = 0;

    for( d_int i = 0; i < song_length; ) {
        byte info = *(byte*)(data + song_offset + i);

        bool   last    = (info & 0x80) != 0;
        d_uint type    = (info & 0x70) >> 4;
        d_uint channel = (info & 0x0F);

        midi_channel = channel;

        // Info has been read.
        i++;

        //printf( "type = %i; channel = %i; ", type, channel );

        switch( type ) {
            case 0: // Release Note
                {
                    d_uint note_number = *(byte*)(data + song_offset + i) & 0x7F;
                    i++;

                    tsf_channel_note_off(sound_font, midi_channel, note_number);
                }
                break;
            case 1: // Play Note
                {
                    byte             note = *(byte*)(data + song_offset + i);
                    boolean volume_enable = note & 0x80;
                    byte      note_number = note & 0x7F;
                    i++;

                    byte volume;

                    if( volume_enable ) {
                        volume = *(byte*)(data + song_offset + i) & 0x7F;
                        channel_last_volumes[channel] = volume;
                        //printf( "set volume;" );
                        i++;
                    }
                    else
                        volume = channel_last_volumes[channel];

                    tsf_channel_note_on(sound_font, midi_channel, note_number, volume * (1./127.));
                }
                break;
            case 2: // Pitch Bend
                {
                    byte pitch_bend = *(byte*)(data + song_offset + i);
                    i++;

                    tsf_channel_set_pitchwheel(sound_font, midi_channel, pitch_bend * 0x40);
                }
                break;
            case 3: // System Event
                {
                    byte controller = *(byte*)(data + song_offset + i) & 0x7F;
                    i++;

                    tsf_channel_midi_control(sound_font, midi_channel, mus_to_midi[controller % MUS_TABLE_LIMIT], 0);
                }
                break;
            case 4: // Controller
                {
                    byte controller = *(byte*)(data + song_offset + i) & 0x7F;
                    i++;
                    byte value = *(byte*)(data + song_offset + i) & 0x7F;
                    i++;
                    //printf( "controller = %i; value = %i;\n", controller, value );

                    if( controller == 0 ) {
                        tsf_channel_set_presetnumber(sound_font, midi_channel, value, (midi_channel == 9));
                    }
                    else {
                        tsf_channel_midi_control(sound_font, midi_channel, mus_to_midi[controller % MUS_TABLE_LIMIT], value);
                    }
                }
                break;
            default:
                printf( "command not recognized at 0x%x;\n", i );
            case 5:
            case 6:
                break;
        }

        d_ulong delay_amount = 0;

        while( last ) {
            last = (*(byte*)(data + song_offset + i) & 0x80) != 0;
            delay_amount = delay_amount * 128 + (*(byte*)(data + song_offset + i) & 0x7F);
            i++;
        }

        if( delay_amount != 0 ) {
            size_t size = (MIDI_SAMPLE_RATE * delay_amount) / MUS_DELAY_RATE;

            midi_buffer = realloc(midi_buffer, 2 * sizeof(short) * (midi_buffer_length + size));
            tsf_render_short(sound_font, midi_buffer + midi_buffer_length, size, 0);

            midi_buffer_length += 2 * size;
        }
    }

    free( midi_buffer );

    return 1;
}

// Is the song playing?
d_int I_QrySongPlaying(d_int handle)
{
    if( handle >= MAX_MUSIC_BUFFERS || handle < 0 )
        return false; // This handle is out of bounds.

    return !music_buffers[handle].paused;
}


// Interrupt handler.
void I_HandleSoundTimer( d_int ignore )
{
    // UNUSED, but required.
    ignore = 0;
    return;
}

// Get the interrupt. Set duration in millisecs.
d_int I_SoundSetTimer( d_int duration_of_tick )
{
    return 0; // There is no need for a timer.
}


// Remove the interrupt. Set duration to zero.
void I_SoundDelTimer()
{
    // No interupt to delete.
}
