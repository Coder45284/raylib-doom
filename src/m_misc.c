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
static const d_char rcsid[] = "$Id: m_misc.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

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
#ifndef O_BINARY
#define O_BINARY 0
#endif

boolean
M_WriteFile
( d_char const*	name,
  void*		source,
  d_int		length )
{
    d_int		handle;
    d_int		count;
	
    handle = open ( name, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666);

    if (handle == -1)
	return false;

    count = write (handle, source, length);
    close (handle);
	
    if (count < length)
	return false;
		
    return true;
}


//
// M_ReadFile
//
d_int
M_ReadFile
( d_char const*	name,
  byte**	buffer )
{
    d_int	handle, count, length;
    struct stat	fileinfo;
    byte		*buf;
	
    handle = open (name, O_RDONLY | O_BINARY, 0666);
    if (handle == -1)
	I_Error ("Couldn't read file %s", name);
    if (fstat (handle,&fileinfo) == -1)
	I_Error ("Couldn't read file %s", name);
    length = fileinfo.st_size;
    buf = Z_Malloc (length, PU_STATIC, NULL);
    count = read (handle, buf, length);
    close (handle);
	
    if (count < length)
	I_Error ("Couldn't read file %s", name);
		
    *buffer = buf;
    return length;
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


// UNIX hack, to be removed.
#ifdef SNDSERV
extern d_char*	sndserver_filename;
extern d_int	mb_used;
#endif

#ifdef LINUX
d_char*		mousetype;
d_char*		mousedev;
#endif

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
    

#ifdef NORMALUNIX
    {"key_right",&key_right, KEY_RIGHTARROW},
    {"key_left",&key_left, KEY_LEFTARROW},
    {"key_up",&key_up, KEY_UPARROW},
    {"key_down",&key_down, KEY_DOWNARROW},
    {"key_strafeleft",&key_strafeleft, ','},
    {"key_straferight",&key_straferight, '.'},

    {"key_fire",&key_fire, KEY_RCTRL},
    {"key_use",&key_use, ' '},
    {"key_strafe",&key_strafe, KEY_RALT},
    {"key_speed",&key_speed, KEY_RSHIFT},

// UNIX hack, to be removed. 
#ifdef SNDSERV
    {"sndserver", (d_int *) &sndserver_filename, (d_int) "sndserver"},
    {"mb_used", &mb_used, 2},
#endif
    
#endif

#ifdef LINUX
    {"mousedev", (d_int*)&mousedev, (d_int)"/dev/ttyS0"},
    {"mousetype", (d_int*)&mousetype, (d_int)"microsoft"},
#endif

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



    {"usegamma",&usegamma, 0},

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
	if (access(lbmname,0) == -1)
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


