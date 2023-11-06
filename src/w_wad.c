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
//	Handles WAD file header, directory, lump I/O.
//
//-----------------------------------------------------------------------------

#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <assert.h>

#include "doomtype.h"
#include "m_swap.h"
#include "i_system.h"
#include "z_zone.h"

#include "w_wad.h"

#include "raylib.h"

//
// GLOBALS
//

// Location of each lump on disk.
lumpinfo_t*		lumpinfo;
d_int			numlumps;
#define MAX_HANDLES 20
static unsigned char* file_handles[MAX_HANDLES];
static d_int number_of_handles = 0;

void**			lumpcache;

boolean str_case_equal( const d_char *const left, const d_char *const right ) {
    size_t length_left = strlen( left );
    size_t length_right = strlen( right );

    if( length_left != length_right )
        return false;

    for( size_t i = 0; left[i] != '\0'; i++)
        if( toupper(left[i]) != toupper(right[i]) )
            return false;

    return true;
}

void strupr (char* s)
{
    while (*s) { *s = toupper(*s); s++; }
}

void
ExtractFileBase
( d_char*		path,
  d_char*		dest )
{
    d_char*	src;
    d_int		length;

    src = path + strlen(path) - 1;
    
    // back up until a \ or the start
    while (src != path
	   && *(src-1) != '\\'
	   && *(src-1) != '/')
    {
	src--;
    }
    
    // copy up to eight characters
    memset (dest,0,8);
    length = 0;
    
    while (*src && *src != '.')
    {
	if (++length == 9)
	    I_Error ("Filename base of %s >8 chars",path);

	*dest++ = toupper((d_int)*src++);
    }
}





//
// LUMP BASED ROUTINES.
//

//
// W_AddFile
// All files are optional, but at least one file must be
//  found (PWAD, if all required lumps are present).
// Files with a .wad extension are wadlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.
//
// If filename starts with a tilde, the file is handled
//  specially to allow map reloads.
// But: the reload feature is a fragile hack...

d_int			reloadlump;
d_char*			reloadname;


void W_AddFile (d_char *filename)
{
    wadinfo_t   header;
    lumpinfo_t* lump_p;
    d_uint      i;
    d_int       length;
    d_int       startlump;
    filelump_t* fileinfo;
    filelump_t  singleinfo;
    d_int       storehandle;
    
    // open the file and add to directory

    // handle reload indicator.
    if (filename[0] == '~')
    {
        filename++;
        reloadname = filename;
        reloadlump = numlumps;
    }

    unsigned int bytesRead = 0;

    byte* file_data = LoadFileData(filename, &bytesRead);
    byte* file_data_begin = file_data;

    if( file_data == NULL && bytesRead == 0 )
    {
        printf (" couldn't open %s\n",filename);
        return;
    }

    printf (" adding %s\n",filename);
    startlump = numlumps;

    if( !str_case_equal( GetFileExtension(filename) + 1, "wad" ) )
    {
        // single lump file
        fileinfo = &singleinfo;
        singleinfo.filepos = 0;
        singleinfo.size = GetFileLength(filename);
        ExtractFileBase (filename, singleinfo.name);
        numlumps++;
    }
    else 
    {
        // WAD file
        header.identification[0] = file_data[0]; file_data++;
        header.identification[1] = file_data[0]; file_data++;
        header.identification[2] = file_data[0]; file_data++;
        header.identification[3] = file_data[0]; file_data++;
        header.numlumps     = LONG(*((d_int*)file_data)); file_data += sizeof(d_int);
        header.infotableofs = LONG(*((d_int*)file_data)); file_data += sizeof(d_int);

        if (strncmp(header.identification,"IWAD",4))
        {
            // Homebrew levels?
            if (strncmp(header.identification,"PWAD",4))
            {
                I_Error ("Wad file %s doesn't have IWAD or PWAD id\n", filename);
            }

            // ???modifiedgame = true;
        }
        file_data = file_data_begin + header.infotableofs;
        fileinfo = (filelump_t*)file_data;
        numlumps += header.numlumps;
    }

    
    // Fill in lumpinfo
    lumpinfo = realloc (lumpinfo, numlumps * sizeof(lumpinfo_t));

    if (!lumpinfo)
        I_Error ("Couldn't realloc lumpinfo");

    lump_p = &lumpinfo[startlump];

    if( reloadname )
        storehandle = -1;
    else if( number_of_handles < MAX_HANDLES ) {
        storehandle = number_of_handles;

        file_handles[ number_of_handles ] = file_data_begin;
        number_of_handles++;
    }
    else
        I_Error ("Out of handle space!");

    for( i = startlump; i < numlumps; i++, lump_p++, fileinfo++ )
    {
        lump_p->handle = storehandle;
        lump_p->position = LONG(fileinfo->filepos);
        lump_p->size = LONG(fileinfo->size);
        strncpy (lump_p->name, fileinfo->name, 8);
    }

    if (reloadname)
        UnloadFileData( file_data_begin );
}




//
// W_Reload
// Flushes any of the reloadable lumps in memory
//  and reloads the directory.
//
void W_Reload (void)
{
    wadinfo_t   header;
    lumpinfo_t* lump_p;
    d_uint      i;
    d_int       length;
    filelump_t* fileinfo;

    if (!reloadname)
        return;

    unsigned int bytesRead = 0;

    byte* file_data = LoadFileData(reloadname, &bytesRead);
    byte* file_data_begin = file_data;

    if( file_data == NULL && bytesRead == 0 )
        I_Error ("W_Reload: couldn't open %s", reloadname);

    header.identification[0] = file_data[0]; file_data++;
    header.identification[1] = file_data[0]; file_data++;
    header.identification[2] = file_data[0]; file_data++;
    header.identification[3] = file_data[0]; file_data++;
    header.numlumps     = LONG(*((d_int*)file_data)); file_data += sizeof(d_int);
    header.infotableofs = LONG(*((d_int*)file_data)); file_data += sizeof(d_int);
    length = header.numlumps * sizeof(filelump_t);
    fileinfo = (filelump_t*)(file_data_begin + header.infotableofs);
    
    // Fill in lumpinfo
    lump_p = &lumpinfo[reloadlump];

    for( i = reloadlump; i < reloadlump + header.numlumps; i++, lump_p++, fileinfo++ )
    {
        if (lumpcache[i])
            Z_Free (lumpcache[i]);

        lump_p->position = LONG(fileinfo->filepos);
        lump_p->size = LONG(fileinfo->size);
    }

    UnloadFileData( file_data_begin );
}



//
// W_InitMultipleFiles
// Pass a null terminated list of files to use.
// All files are optional, but at least one file
//  must be found.
// Files with a .wad extension are idlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.
// Lump names can appear multiple times.
// The name searcher looks backwards, so a later file
//  does override all earlier ones.
//
void W_InitMultipleFiles (d_char** filenames)
{	
    d_int		size;
    
    // open all the files, load headers, and count lumps
    numlumps = 0;

    // will be realloced as lumps are added
    lumpinfo = malloc(1);

    for ( ; *filenames ; filenames++)
    {
        printf( "%s", *filenames );
        W_AddFile (*filenames);
    }

    if (!numlumps)
        I_Error ("W_InitFiles: no files found");
    
    // set up caching
    size = numlumps * sizeof(*lumpcache);
    lumpcache = malloc (size);
    
    if (!lumpcache)
        I_Error ("Couldn't allocate lumpcache");

    memset (lumpcache,0, size);
}




//
// W_InitFile
// Just initialize from a single file.
//
void W_InitFile (d_char* filename)
{
    d_char*	names[2];

    names[0] = filename;
    names[1] = NULL;
    W_InitMultipleFiles (names);
}



//
// W_NumLumps
//
d_int W_NumLumps (void)
{
    return numlumps;
}



//
// W_CheckNumForName
// Returns -1 if name not found.
//

d_int W_CheckNumForName (d_char* name)
{
	d_char capital_name[9];
    lumpinfo_t* lump_p;

    for( byte i = 0; i < 9; i++ )
        capital_name[i] = 0;

    // case insensitive
    for( byte i = 0; name[i] != '\0' && i < 9; i++ )
        capital_name[i] = toupper( name[i] );

    // scan backwards so patch lump files take precedence
    lump_p = lumpinfo + numlumps;

    while (lump_p-- != lumpinfo)
    {
        if ( strncmp(capital_name, lump_p->name, 8) == 0 )
        {
            return lump_p - lumpinfo;
        }
    }

    // TFB. Not found.
    return -1;
}




//
// W_GetNumForName
// Calls W_CheckNumForName, but bombs out if not found.
//
d_int W_GetNumForName (d_char* name)
{
    d_int	i;

    i = W_CheckNumForName (name);
    
    if (i == -1)
      I_Error ("W_GetNumForName: %s not found!", name);
      
    return i;
}


//
// W_LumpLength
// Returns the buffer size needed to load the given lump.
//
d_int W_LumpLength (d_int lump)
{
    if (lump >= numlumps)
        I_Error ("W_LumpLength: %i >= numlumps",lump);

    return lumpinfo[lump].size;
}



//
// W_ReadLump
// Loads the lump into the given buffer,
//  which must be >= W_LumpLength().
//
void
W_ReadLump( d_int lump, void* dest )
{
    d_int       c;
    lumpinfo_t* l;
    byte* file_data;

    if(lump >= numlumps)
        I_Error("W_ReadLump: %i >= numlumps", lump);

    l = lumpinfo + lump;

    // ??? I_BeginRead ();

    if(l->handle == -1)
    {
        unsigned int bytesRead = 0;

        file_data = LoadFileData(reloadname, &bytesRead);

        if( file_data == NULL && bytesRead == 0 )
            I_Error ("W_ReadLump: couldn't open %s", reloadname);
    }
    else
        file_data = file_handles[ l->handle ];

    // TODO Add bounds checking.
    dest = memcpy(dest, file_data + l->position, l->size);

    c = l->size;

    if(c < l->size)
        I_Error("W_ReadLump: only read %i of %i on lump %i", c, l->size, lump);

    if(l->handle == -1)
        UnloadFileData( file_data );

    // ??? I_EndRead ();
}




//
// W_CacheLumpNum
//
void*
W_CacheLumpNum
( d_int		lump,
  d_int		tag )
{
    byte*	ptr;

    if ((d_uint)lump >= numlumps)
	I_Error ("W_CacheLumpNum: %i >= numlumps",lump);
		
    if (!lumpcache[lump])
    {
	// read the lump in
	
	//printf ("cache miss on lump %i\n",lump);
	ptr = Z_Malloc (W_LumpLength (lump), tag, &lumpcache[lump]);
	W_ReadLump (lump, lumpcache[lump]);
    }
    else
    {
	//printf ("cache hit on lump %i\n",lump);
	Z_ChangeTag (lumpcache[lump],tag);
    }
	
    return lumpcache[lump];
}



//
// W_CacheLumpName
//
void*
W_CacheLumpName
( d_char*		name,
  d_int		tag )
{
    return W_CacheLumpNum (W_GetNumForName(name), tag);
}


//
// W_Profile
//
d_int		info[2500][10];
d_int		profilecount;

void W_Profile (void)
{
    d_int		i;
    memblock_t*	block;
    void*	ptr;
    d_char	ch;
    FILE*	f;
    d_int		j;
    d_char	name[9];
	
	
    for (i=0 ; i<numlumps ; i++)
    {	
	ptr = lumpcache[i];
	if (!ptr)
	{
	    ch = ' ';
	    continue;
	}
	else
	{
	    block = (memblock_t *) ( (byte *)ptr - sizeof(memblock_t));
	    if (block->tag < PU_PURGELEVEL)
		ch = 'S';
	    else
		ch = 'P';
	}
	info[i][profilecount] = ch;
    }
    profilecount++;
	
    f = fopen ("waddump.txt","w");
    name[8] = 0;

    for (i=0 ; i<numlumps ; i++)
    {
	memcpy (name,lumpinfo[i].name,8);

	for (j=0 ; j<8 ; j++)
	    if (!name[j])
		break;

	for ( ; j<8 ; j++)
	    name[j] = ' ';

	fprintf (f,"%s ",name);

	for (j=0 ; j<profilecount ; j++)
	    fprintf (f,"    %c",info[i][j]);

	fprintf (f,"\n");
    }
    fclose (f);
}


