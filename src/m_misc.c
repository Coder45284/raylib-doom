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
// $Log:$
//
// DESCRIPTION:
//	Main loop menu stuff.
//	Default Config File.
//	PCX Screenshots.
//
//-----------------------------------------------------------------------------

#include "doomtype.h"

#include <stdlib.h>
#include <ctype.h>

#include "doomdef.h"

#include "z_zone.h"

#include "m_swap.h"
#include "m_argv.h"

#include "w_wad.h"

#include "i_system.h"
#include "i_video.h"
#include "v_video.h"

#include "hu_stuff.h"

// State.
#include "doomstat.h"

// Data.
#include "dstrings.h"

#include "m_misc.h"

#include "raylib.h"

//
// M_DrawText
// Returns the final X coordinate
// HU_Init must have been called to init the font
//
extern patch_t*		hu_font[HU_FONTSIZE];

d_int
M_DrawText
( d_int		x,
  d_int		y,
  boolean	direct,
  d_char*		string )
{
    d_int 	c;
    d_int		w;

    while (*string)
    {
	c = toupper(*string) - HU_FONTSTART;
	string++;
	if (c < 0 || c> HU_FONTSIZE)
	{
	    x += 4;
	    continue;
	}
		
	w = SHORT (hu_font[c]->width);
	if (x+w > SCREENWIDTH)
	    break;
	if (direct)
	    V_DrawPatchDirect(x, y, 0, hu_font[c]);
	else
	    V_DrawPatch(x, y, 0, hu_font[c]);
	x+=w;
    }

    return x;
}




//
// M_WriteFile
//

boolean M_WriteFile( d_char const* name, void* source, d_int length )
{
    if( !SaveFileData(name, source, length) )
        return false;

    if( GetFileLength(name) < length )
        return false;

    return true;
}


//
// M_ReadFile
//
d_int M_ReadFile ( d_char const* name, byte** buffer )
{
    byte        *readFile, *buf;
    unsigned int fileLength;

    readFile = LoadFileData( name, &fileLength );

    if(readFile == NULL)
        I_Error ("Couldn't read file %s", name);

    buf = Z_Malloc (fileLength, PU_STATIC, NULL);

    if( buf == NULL ) {
        UnloadFileData( readFile );
        I_Error ("M_ReadFile: Z_Malloc ran out of space for \"%s\"", name);

    }
    memcpy(buf, readFile, fileLength);
    UnloadFileData( readFile );

    *buffer = buf;
    return fileLength;
}


//
// DEFAULTS
//
d_int		usemouse;
d_int		usejoystick;

extern d_int	key_right;
extern d_int	key_left;
extern d_int	key_up;
extern d_int	key_down;

extern d_int	key_strafeleft;
extern d_int	key_straferight;

extern d_int	key_fire;
extern d_int	key_use;
extern d_int	key_strafe;
extern d_int	key_speed;

extern d_int	mousebfire;
extern d_int	mousebstrafe;
extern d_int	mousebforward;

extern d_int	joybfire;
extern d_int	joybstrafe;
extern d_int	joybuse;
extern d_int	joybspeed;

extern d_int	viewwidth;
extern d_int	viewheight;

extern d_int	mouseSensitivity;
extern d_int	showMessages;

extern d_int	detailLevel;

extern d_int	screenblocks;

extern d_int	showMessages;

// machine-independent sound params
extern	d_int	numChannels;

extern d_char*	chat_macros[];



typedef struct
{
    d_char*  name;
    d_int*   location;
    d_intptr defaultvalue;
    d_int    scantranslate;		// PC scan code hack
    d_int    untranslated;		// lousy hack
} default_t;

default_t	defaults[] =
{
    {"mouse_sensitivity",&mouseSensitivity, 5},
    {"sfx_volume",&snd_SfxVolume, 8},
    {"music_volume",&snd_MusicVolume, 8},
    {"show_messages",&showMessages, 1},
    
    {"key_right",&key_right, D_KEY_RIGHTARROW},
    {"key_left",&key_left, D_KEY_LEFTARROW},
    {"key_up",&key_up, 'w'},
    {"key_down",&key_down, 's'},
    {"key_strafeleft",&key_strafeleft, 'a'},
    {"key_straferight",&key_straferight, 'd'},

    {"key_fire",&key_fire, 'e'},
    {"key_use",&key_use, ' '},
    {"key_strafe",&key_strafe, D_KEY_RALT},
    {"key_speed",&key_speed, D_KEY_RSHIFT},

    {"use_mouse",&usemouse, 1},
    {"mouseb_fire",&mousebfire,0},
    {"mouseb_strafe",&mousebstrafe,1},
    {"mouseb_forward",&mousebforward,2},

    {"use_joystick",&usejoystick, 0},
    {"joyb_fire",&joybfire,0},
    {"joyb_strafe",&joybstrafe,1},
    {"joyb_use",&joybuse,3},
    {"joyb_speed",&joybspeed,2},

    {"screenblocks",&screenblocks, 9},
    {"detaillevel",&detailLevel, 0},

    {"snd_channels",&numChannels, 3},

    {"chatmacro0", (d_int *) &chat_macros[0], (d_intptr) HUSTR_CHATMACRO0 },
    {"chatmacro1", (d_int *) &chat_macros[1], (d_intptr) HUSTR_CHATMACRO1 },
    {"chatmacro2", (d_int *) &chat_macros[2], (d_intptr) HUSTR_CHATMACRO2 },
    {"chatmacro3", (d_int *) &chat_macros[3], (d_intptr) HUSTR_CHATMACRO3 },
    {"chatmacro4", (d_int *) &chat_macros[4], (d_intptr) HUSTR_CHATMACRO4 },
    {"chatmacro5", (d_int *) &chat_macros[5], (d_intptr) HUSTR_CHATMACRO5 },
    {"chatmacro6", (d_int *) &chat_macros[6], (d_intptr) HUSTR_CHATMACRO6 },
    {"chatmacro7", (d_int *) &chat_macros[7], (d_intptr) HUSTR_CHATMACRO7 },
    {"chatmacro8", (d_int *) &chat_macros[8], (d_intptr) HUSTR_CHATMACRO8 },
    {"chatmacro9", (d_int *) &chat_macros[9], (d_intptr) HUSTR_CHATMACRO9 }

};

d_int	numdefaults;
d_char*	defaultfile;


//
// M_SaveDefaults
//
void M_SaveDefaults (void)
{
    d_int		i;
    d_int		v;
    FILE*	f;
	
    f = fopen (defaultfile, "w");
    if (!f)
	return; // can't write the file, but don't complain
		
    for (i=0 ; i<numdefaults ; i++)
    {
	if (defaults[i].defaultvalue > -0xfff
	    && defaults[i].defaultvalue < 0xfff)
	{
	    v = *defaults[i].location;
	    fprintf (f,"%s\t\t%i\n",defaults[i].name,v);
	} else {
	    fprintf (f,"%s\t\t\"%s\"\n",defaults[i].name,
		     * (d_char **) (defaults[i].location));
	}
    }
	
    fclose (f);
}


//
// M_LoadDefaults
//
extern byte	scantokey[128];

void M_LoadDefaults (void)
{
    d_int		i;
    d_int		len;
    FILE*	f;
    d_char	def[80];
    d_char	strparm[100];
    d_char*	newstring;
    d_int		parm;
    boolean	isstring;
    
    // set everything to base values
    numdefaults = sizeof(defaults)/sizeof(defaults[0]);
    for (i=0 ; i<numdefaults ; i++)
	*defaults[i].location = defaults[i].defaultvalue;
    
    // check for a custom default file
    i = M_CheckParm ("-config");
    if (i && i<myargc-1)
    {
	defaultfile = myargv[i+1];
	printf ("	default file: %s\n",defaultfile);
    }
    else
	defaultfile = basedefault;
    
    // read the file in, overriding any set defaults
    f = fopen (defaultfile, "r");
    if (f)
    {
	while (!feof(f))
	{
	    isstring = false;
	    if (fscanf (f, "%79s %[^\n]\n", def, strparm) == 2)
	    {
		if (strparm[0] == '"')
		{
		    // get a string default
		    isstring = true;
		    len = strlen(strparm);
		    newstring = (d_char *) malloc(len);
		    strparm[len-1] = 0;
		    strcpy(newstring, strparm+1);
		}
		else if (strparm[0] == '0' && strparm[1] == 'x')
		    sscanf(strparm+2, "%x", &parm);
		else
		    sscanf(strparm, "%i", &parm);
		for (i=0 ; i<numdefaults ; i++)
		    if (!strcmp(def, defaults[i].name))
		    {
			if (!isstring)
			    *defaults[i].location = parm;
			else
			    *defaults[i].location =
				(d_intptr) newstring;
			break;
		    }
	    }
	}
		
	fclose (f);
    }
}


//
// SCREEN SHOTS
//


typedef struct
{
    d_char		manufacturer;
    d_char		version;
    d_char		encoding;
    d_char		bits_per_pixel;

    d_ushort	xmin;
    d_ushort	ymin;
    d_ushort	xmax;
    d_ushort	ymax;
    
    d_ushort	hres;
    d_ushort	vres;

    byte	palette[48];
    
    d_char		reserved;
    d_char		color_planes;
    d_ushort	bytes_per_line;
    d_ushort	palette_type;
    
    d_char		filler[58];
    byte	data;		// unbounded
} pcx_t;


//
// WritePCXfile
//
void
WritePCXfile
( d_char*		filename,
  byte*		data,
  d_int		width,
  d_int		height,
  byte*		palette )
{
    d_int		i;
    d_int		length;
    pcx_t*	pcx;
    byte*	pack;
	
    pcx = Z_Malloc (width*height*2+1000, PU_STATIC, NULL);

    pcx->manufacturer = 0x0a;		// PCX id
    pcx->version = 5;			// 256 color
    pcx->encoding = 1;			// uncompressed
    pcx->bits_per_pixel = 8;		// 256 color
    pcx->xmin = 0;
    pcx->ymin = 0;
    pcx->xmax = SHORT(width-1);
    pcx->ymax = SHORT(height-1);
    pcx->hres = SHORT(width);
    pcx->vres = SHORT(height);
    memset (pcx->palette,0,sizeof(pcx->palette));
    pcx->color_planes = 1;		// chunky image
    pcx->bytes_per_line = SHORT(width);
    pcx->palette_type = SHORT(2);	// not a grey scale
    memset (pcx->filler,0,sizeof(pcx->filler));


    // pack the image
    pack = &pcx->data;
	
    for (i=0 ; i<width*height ; i++)
    {
	if ( (*data & 0xc0) != 0xc0)
	    *pack++ = *data++;
	else
	{
	    *pack++ = 0xc1;
	    *pack++ = *data++;
	}
    }
    
    // write the palette
    *pack++ = 0x0c;	// palette ID byte
    for (i=0 ; i<768 ; i++)
	*pack++ = *palette++;
    
    // write output file
    length = pack - (byte *)pcx;
    M_WriteFile (filename, pcx, length);

    Z_Free (pcx);
}


//
// M_ScreenShot
//
void M_ScreenShot (void)
{
    d_int		i;
    byte*	linear;
    d_char	lbmname[12];
    
    // munge planar buffer to linear
    linear = screens[2];
    I_ReadScreen (linear);
    
    // find a file name to save it to
    strcpy(lbmname,"DOOM00.pcx");

    for (i=0 ; i<=99 ; i++)
    {
        lbmname[4] = i/10 + '0';
        lbmname[5] = i%10 + '0';
        if( !FileExists(lbmname) )
            break;	// file doesn't exist
    }
    if (i==100)
        I_Error ("M_ScreenShot: Couldn't create a PCX");
    
    // save the pcx file
    WritePCXfile (lbmname, linear,
		  SCREENWIDTH, SCREENHEIGHT,
		  W_CacheLumpName ("PLAYPAL",PU_CACHE));
	
    players[consoleplayer].message = "screen shot";
}


