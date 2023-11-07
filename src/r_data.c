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
// Revision 1.3  1997/01/29 20:10
// DESCRIPTION:
//	Preparation of data for rendering,
//	generation of lookups, caching, retrieval by name.
//
//-----------------------------------------------------------------------------

#include "i_system.h"
#include "z_zone.h"

#include "m_swap.h"

#include "w_wad.h"

#include "doomdef.h"
#include "r_local.h"
#include "p_local.h"

#include "doomstat.h"
#include "r_sky.h"

#include "r_data.h"

#include  <alloca.h>

#include <strings.h>

//
// Graphics.
// DOOM graphics for walls and sprites
// is stored in vertical runs of opaque pixels (posts).
// A column is composed of zero or more posts,
// a patch or sprite is composed of zero or more columns.
// 



//
// Texture definition.
// Each texture is composed of one or more patches,
// with patches being lumps stored in the WAD.
// The lumps are referenced by number, and patched
// into the rectangular texture space using origin
// and possibly other attributes.
//
typedef struct
{
    d_short originx;
    d_short originy;
    d_short patch;
    d_short stepdir;
    d_short colormap;
} mappatch_t;


//
// Texture definition.
// A DOOM wall texture is a list of patches
// which are to be combined in a predefined order.
//
typedef struct
{
    d_char     name[8];
    boolean    masked;
    d_short    width;
    d_short    height;
    d_int      columndirectory; // OBSOLETE
    d_short    patchcount;
    mappatch_t patches[1];
} maptexture_t;


// A single patch from a texture definition,
//  basically a rectangular area within
//  the texture rectangle.
typedef struct
{
    // Block origin (always UL),
    // which has allready accounted
    // for the internal origin of the patch.
    d_int		originx;
    d_int		originy;
    d_int		patch;
} texpatch_t;


// A maptexturedef_t describes a rectangular texture,
//  which is composed of one or more mappatch_t structures
//  that arrange graphic patches.
typedef struct
{
    // Keep name for switch changing, etc.
    d_char	name[8];
    d_short	width;
    d_short	height;
    
    // All the patches[patchcount]
    //  are drawn back to front into the cached texture.
    d_short	patchcount;
    texpatch_t	patches[1];		
    
} texture_t;



d_int		firstflat;
d_int		lastflat;
d_int		numflats;

d_int		firstpatch;
d_int		lastpatch;
d_int		numpatches;

d_int		firstspritelump;
d_int		lastspritelump;
d_int		numspritelumps;

d_int		numtextures;
texture_t**	textures;


d_int*			texturewidthmask;
// needed for texture pegging
fixed_t*		textureheight;		
d_int*			texturecompositesize;
d_short**			texturecolumnlump;
d_ushort**	texturecolumnofs;
byte**			texturecomposite;

// for global animation
d_int*		flattranslation;
d_int*		texturetranslation;

// needed for pre rendering
fixed_t*	spritewidth;	
fixed_t*	spriteoffset;
fixed_t*	spritetopoffset;

lighttable_t	*colormaps;


//
// MAPTEXTURE_T CACHING
// When a texture is first needed,
//  it counts the number of composite columns
//  required in the texture and allocates space
//  for a column directory and any new columns.
// The directory will simply point inside other patches
//  if there is only one patch in a given column,
//  but any columns with multiple patches
//  will have new column_ts generated.
//



//
// R_DrawColumnInCache
// Clip and draw a column
//  from a patch into a cached post.
//
void R_DrawColumnInCache ( column_t* patch, byte* cache, d_int originy, d_int cacheheight )
{
    d_int		count;
    d_int		position;
    byte*	source;
    byte*	dest;
	
    dest = (byte *)cache + 3;
	
    while (patch->topdelta != 0xff)
    {
	source = (byte *)patch + 3;
	count = patch->length;
	position = originy + patch->topdelta;

	if (position < 0)
	{
	    count += position;
	    position = 0;
	}

	if (position + count > cacheheight)
	    count = cacheheight - position;

	if (count > 0)
	    memcpy (cache + position, source, count);
		
	patch = (column_t *)(  (byte *)patch + patch->length + 4); 
    }
}



//
// R_GenerateComposite
// Using the texture definition,
//  the composite texture is created from the patches,
//  and each column is cached.
//
void R_GenerateComposite (d_int texnum)
{
    byte*		block;
    texture_t*		texture;
    texpatch_t*		patch;	
    patch_t*		realpatch;
    d_int			x;
    d_int			x1;
    d_int			x2;
    d_int			i;
    column_t*		patchcol;
    d_short*		collump;
    d_ushort*	colofs;
	
    texture = textures[texnum];

    block = Z_Malloc (texturecompositesize[texnum],
		      PU_STATIC, 
		      &texturecomposite[texnum]);	

    collump = texturecolumnlump[texnum];
    colofs = texturecolumnofs[texnum];
    
    // Composite the columns together.
    patch = texture->patches;
		
    for (i=0 , patch = texture->patches;
	 i<texture->patchcount;
	 i++, patch++)
    {
	realpatch = W_CacheLumpNum (patch->patch, PU_CACHE);
	x1 = patch->originx;
	x2 = x1 + SHORT(realpatch->width);

	if (x1<0)
	    x = 0;
	else
	    x = x1;
	
	if (x2 > texture->width)
	    x2 = texture->width;

	for ( ; x<x2 ; x++)
	{
	    // Column does not have multiple patches?
	    if (collump[x] >= 0)
		continue;
	    
	    patchcol = (column_t *)((byte *)realpatch
				    + LONG(realpatch->columnofs[x-x1]));
	    R_DrawColumnInCache (patchcol,
				 block + colofs[x],
				 patch->originy,
				 texture->height);
	}
						
    }

    // Now that the texture has been built in column cache,
    //  it is purgable from zone memory.
    Z_ChangeTag (block, PU_CACHE);
}



//
// R_GenerateLookup
//
void R_GenerateLookup (d_int texnum)
{
    texture_t*		texture;
    byte*		patchcount;	// patchcount[texture->width]
    texpatch_t*		patch;	
    patch_t*		realpatch;
    d_int			x;
    d_int			x1;
    d_int			x2;
    d_int			i;
    d_short*		collump;
    d_ushort*	colofs;
	
    texture = textures[texnum];

    // Composited texture not created yet.
    texturecomposite[texnum] = 0;
    
    texturecompositesize[texnum] = 0;
    collump = texturecolumnlump[texnum];
    colofs = texturecolumnofs[texnum];
    
    // Now count the number of columns
    //  that are covered by more than one patch.
    // Fill in the lump / offset, so columns
    //  with only a single patch are all done.
    patchcount = (byte *)alloca (texture->width);
    memset (patchcount, 0, texture->width);
    patch = texture->patches;
		
    for (i=0 , patch = texture->patches;
	 i<texture->patchcount;
	 i++, patch++)
    {
	realpatch = W_CacheLumpNum (patch->patch, PU_CACHE);
	x1 = patch->originx;
	x2 = x1 + SHORT(realpatch->width);
	
	if (x1 < 0)
	    x = 0;
	else
	    x = x1;

	if (x2 > texture->width)
	    x2 = texture->width;
	for ( ; x<x2 ; x++)
	{
	    patchcount[x]++;
	    collump[x] = patch->patch;
	    colofs[x] = LONG(realpatch->columnofs[x-x1])+3;
	}
    }
	
    for (x=0 ; x<texture->width ; x++)
    {
	if (!patchcount[x])
	{
	    printf ("R_GenerateLookup: column without a patch (%s)\n",
		    texture->name);
	    return;
	}
	// I_Error ("R_GenerateLookup: column without a patch");
	
	if (patchcount[x] > 1)
	{
	    // Use the cached block.
	    collump[x] = -1;	
	    colofs[x] = texturecompositesize[texnum];
	    
	    if (texturecompositesize[texnum] > 0x10000-texture->height)
	    {
		I_Error ("R_GenerateLookup: texture %i is >64k",
			 texnum);
	    }
	    
	    texturecompositesize[texnum] += texture->height;
	}
    }	
}




//
// R_GetColumn
//
byte* R_GetColumn ( d_int tex, d_int col ) {
    d_int		lump;
    d_int		ofs;
	
    col &= texturewidthmask[tex];
    lump = texturecolumnlump[tex][col];
    ofs = texturecolumnofs[tex][col];
    
    if (lump > 0)
        return (byte *)W_CacheLumpNum(lump,PU_CACHE)+ofs;

    if (!texturecomposite[tex])
        R_GenerateComposite (tex);

    return texturecomposite[tex] + ofs;
}




//
// R_InitTextures
// Initializes the texture list
//  with the textures from the world map.
//
void R_InitTextures (void)
{
    maptexture_t* mtexture;
    texture_t*    texture;
    mappatch_t*   mpatch;
    texpatch_t*   patch;

    d_int i;
    d_int j;

    d_int* maptex;
    d_int* maptex2;
    d_int* maptex1;
    
    d_char name[9];
    d_char* names;
    d_char* name_p;

    d_int* patchlookup;
    
    d_int totalwidth;
    d_int nummappatches;
    d_int offset;
    d_int maxoff;
    d_int maxoff2;
    d_int numtextures1;
    d_int numtextures2;

    d_int* directory;
    
    d_int temp1;
    d_int temp2;
    d_int temp3;
    
    // Load the patch names from pnames.lmp.
    name[8] = 0;	
    names = W_CacheLumpName ("PNAMES", PU_STATIC);
    nummappatches = LONG ( *((d_int *)names) );

    name_p = names+4;
    patchlookup = alloca( nummappatches * sizeof(*patchlookup) );
    
    for (i=0 ; i<nummappatches ; i++)
    {
        strncpy (name,name_p+i*8, 8);
        patchlookup[i] = W_CheckNumForName (name);
        //printf( "\npatchlookup[%i] size = %i.\n", i, patchlookup[i] );
    }
    Z_Free (names);
    
    // Load the map texture definitions from textures.lmp.
    // The data is contained in one or two lumps,
    //  TEXTURE1 for shareware, plus TEXTURE2 for commercial.
    maptex = maptex1 = W_CacheLumpName ("TEXTURE1", PU_STATIC);
    numtextures1 = LONG(*maptex);
    maxoff = W_LumpLength (W_GetNumForName ("TEXTURE1"));
    directory = maptex + 1;

    if (W_CheckNumForName ("TEXTURE2") != -1)
    {
        maptex2 = W_CacheLumpName ("TEXTURE2", PU_STATIC);
        numtextures2 = LONG(*maptex2);
        maxoff2 = W_LumpLength (W_GetNumForName ("TEXTURE2"));
    }
    else
    {
        maptex2 = NULL;
        numtextures2 = 0;
        maxoff2 = 0;
    }
    numtextures = numtextures1 + numtextures2;

    textures = Z_Malloc (numtextures * sizeof(*textures), PU_STATIC, 0);
    texturecolumnlump = Z_Malloc (numtextures * sizeof(*texturecolumnlump), PU_STATIC, 0);
    texturecolumnofs = Z_Malloc (numtextures * sizeof(*texturecolumnofs), PU_STATIC, 0);
    texturecomposite = Z_Malloc (numtextures * sizeof(*texturecomposite), PU_STATIC, 0);
    texturecompositesize = Z_Malloc (numtextures * sizeof(*texturecompositesize), PU_STATIC, 0);
    texturewidthmask = Z_Malloc (numtextures * sizeof(*texturewidthmask), PU_STATIC, 0);
    textureheight = Z_Malloc (numtextures * sizeof(*textureheight), PU_STATIC, 0);

    totalwidth = 0;
    
    //	Really complex printing shit...
    temp1 = W_GetNumForName ("S_START");  // P_???????
    temp2 = W_GetNumForName ("S_END") - 1;
    temp3 = ((temp2-temp1+63)/64) + ((numtextures+63)/64);
    printf("[");
    for (i = 0; i < temp3; i++)
        printf(" ");
    printf("         ]");
    for (i = 0; i < temp3; i++)
        printf("\x8");
    printf("\x8\x8\x8\x8\x8\x8\x8\x8\x8\x8");

    for (i=0 ; i<numtextures ; i++, directory++)
    {
        if (!(i&63))
            printf (".");

        if (i == numtextures1)
        {
            // Start looking in second texture file.
            maptex = maptex2;
            maxoff = maxoff2;
            directory = maptex+1;
        }

        offset = LONG(*directory);

        if (offset > maxoff)
            I_Error ("R_InitTextures: bad texture directory");

        mtexture = (maptexture_t *) ( (byte *)maptex + offset);

        texture = textures[i] =
            Z_Malloc (sizeof(texture_t)
                + sizeof(texpatch_t)*(SHORT(mtexture->patchcount)-1),
                PU_STATIC, 0);

        texture->width = SHORT(mtexture->width);
        texture->height = SHORT(mtexture->height);
        texture->patchcount = SHORT(mtexture->patchcount);

        memcpy (texture->name, mtexture->name, sizeof(texture->name));
        mpatch = &mtexture->patches[0];
        patch = &texture->patches[0];

        for (j=0 ; j<texture->patchcount ; j++, mpatch++, patch++)
        {
            patch->originx = SHORT(mpatch->originx);
            patch->originy = SHORT(mpatch->originy);

            patch->patch = patchlookup[SHORT(mpatch->patch)];
            if (patch->patch == -1)
            {
                I_Error ("R_InitTextures: Missing patch in texture %s",
                        texture->name);
            }
        }
        texturecolumnlump[i] = Z_Malloc (texture->width*2, PU_STATIC,0);
        texturecolumnofs[i] = Z_Malloc (texture->width*2, PU_STATIC,0);

        j = 1;
        while (j*2 <= texture->width)
            j<<=1;

        texturewidthmask[i] = j-1;
        textureheight[i] = texture->height<<FRACBITS;

        totalwidth += texture->width;
    }

    Z_Free (maptex1);
    if (maptex2)
        Z_Free (maptex2);
    
    // Precalculate whatever possible.	
    for (i=0 ; i<numtextures ; i++)
        R_GenerateLookup (i);
    
    // Create translation table for global animation.
    texturetranslation = Z_Malloc ((numtextures+1)*4, PU_STATIC, 0);
    
    for (i=0 ; i<numtextures ; i++)
        texturetranslation[i] = i;
}



//
// R_InitFlats
//
void R_InitFlats (void)
{
    d_int		i;
	
    firstflat = W_GetNumForName ("F_START") + 1;
    lastflat = W_GetNumForName ("F_END") - 1;
    numflats = lastflat - firstflat + 1;
	
    // Create translation table for global animation.
    flattranslation = Z_Malloc ((numflats+1)*4, PU_STATIC, 0);
    
    for (i=0 ; i<numflats ; i++)
	flattranslation[i] = i;
}


//
// R_InitSpriteLumps
// Finds the width and hoffset of all sprites in the wad,
//  so the sprite does not need to be cached completely
//  just for having the header info ready during rendering.
//
void R_InitSpriteLumps (void)
{
    d_int		i;
    patch_t	*patch;
	
    firstspritelump = W_GetNumForName ("S_START") + 1;
    lastspritelump = W_GetNumForName ("S_END") - 1;
    
    numspritelumps = lastspritelump - firstspritelump + 1;
    spritewidth = Z_Malloc (numspritelumps*4, PU_STATIC, 0);
    spriteoffset = Z_Malloc (numspritelumps*4, PU_STATIC, 0);
    spritetopoffset = Z_Malloc (numspritelumps*4, PU_STATIC, 0);
	
    for (i=0 ; i< numspritelumps ; i++)
    {
	if (!(i&63))
	    printf (".");

	patch = W_CacheLumpNum (firstspritelump+i, PU_CACHE);
	spritewidth[i] = SHORT(patch->width)<<FRACBITS;
	spriteoffset[i] = SHORT(patch->leftoffset)<<FRACBITS;
	spritetopoffset[i] = SHORT(patch->topoffset)<<FRACBITS;
    }
}



//
// R_InitColormaps
//
void R_InitColormaps (void)
{
    d_int	lump, length;
    
    // Load in the light tables, 
    //  256 byte align tables.
    lump = W_GetNumForName("COLORMAP"); 
    length = W_LumpLength (lump) + 255; 
    colormaps = Z_Malloc (length, PU_STATIC, 0); 
    colormaps = (byte *)( ((d_intptr)colormaps + 255)&~0xff);
    W_ReadLump (lump,colormaps); 
}



//
// R_InitData
// Locates all the lumps
//  that will be used by all views
// Must be called after W_Init.
//
void R_InitData (void)
{
    R_InitTextures ();
    printf ("\nInitTextures");
    R_InitFlats ();
    printf ("\nInitFlats");
    R_InitSpriteLumps ();
    printf ("\nInitSprites");
    R_InitColormaps ();
    printf ("\nInitColormaps");
}



//
// R_FlatNumForName
// Retrieval, get a flat number for a flat name.
//
d_int R_FlatNumForName (d_char* name)
{
    d_int		i;
    d_char	namet[9];

    i = W_CheckNumForName (name);

    if (i == -1)
    {
        namet[8] = 0;
        memcpy (namet, name,8);
        I_Error ("R_FlatNumForName: %s not found",namet);
    }
    return i - firstflat;
}




//
// R_CheckTextureNumForName
// Check whether texture is available.
// Filter out NoTexture indicator.
//
d_int	R_CheckTextureNumForName (d_char *name)
{
    d_int		i;

    // "NoTexture" marker.
    if (name[0] == '-')
        return 0;

    for (i=0 ; i<numtextures ; i++)
        if (!strncasecmp (textures[i]->name, name, 8) )
            return i;

    return -1;
}



//
// R_TextureNumForName
// Calls R_CheckTextureNumForName,
//  aborts with error message.
//
d_int	R_TextureNumForName (d_char* name)
{
    d_int i;

    i = R_CheckTextureNumForName (name);

    if ( i == -1 )
    {
        I_Error ("R_TextureNumForName: %s not found", name);
    }
    return i;
}




//
// R_PrecacheLevel
// Preloads all relevant graphics for the level.
//
d_int		flatmemory;
d_int		texturememory;
d_int		spritememory;

void R_PrecacheLevel (void)
{
    d_char*		flatpresent;
    d_char*		texturepresent;
    d_char*		spritepresent;

    d_int			i;
    d_int			j;
    d_int			k;
    d_int			lump;
    
    texture_t*		texture;
    thinker_t*		th;
    spriteframe_t*	sf;

    if (demoplayback)
        return;
    
    // Precache flats.
    flatpresent = alloca(numflats);
    memset (flatpresent,0,numflats);	

    for (i=0 ; i<numsectors ; i++)
    {
        flatpresent[sectors[i].floorpic] = 1;
        flatpresent[sectors[i].ceilingpic] = 1;
    }
	
    flatmemory = 0;

    for (i=0 ; i<numflats ; i++)
    {
        if (flatpresent[i])
        {
            lump = firstflat + i;
            flatmemory += lumpinfo[lump].size;
            W_CacheLumpNum(lump, PU_CACHE);
        }
    }
    
    // Precache textures.
    texturepresent = alloca(numtextures);
    memset (texturepresent,0, numtextures);
	
    for (i=0 ; i<numsides ; i++)
    {
        texturepresent[sides[i].toptexture] = 1;
        texturepresent[sides[i].midtexture] = 1;
        texturepresent[sides[i].bottomtexture] = 1;
    }

    // Sky texture is always present.
    // Note that F_SKY1 is the name used to
    //  indicate a sky floor/ceiling as a flat,
    //  while the sky texture is stored like
    //  a wall texture, with an episode dependend
    //  name.
    texturepresent[skytexture] = 1;
	
    texturememory = 0;
    for (i=0 ; i<numtextures ; i++)
    {
        if (!texturepresent[i])
            continue;

        texture = textures[i];

        for (j=0 ; j<texture->patchcount ; j++)
        {
            lump = texture->patches[j].patch;
            texturememory += lumpinfo[lump].size;
            W_CacheLumpNum(lump , PU_CACHE);
        }
    }
    
    // Precache sprites.
    spritepresent = alloca(numsprites);
    memset (spritepresent,0, numsprites);
	
    for (th = thinkercap.next ; th != &thinkercap ; th=th->next)
    {
        if (th->function.acp1 == (actionf_p1)P_MobjThinker)
            spritepresent[((mobj_t *)th)->sprite] = 1;
    }
	
    spritememory = 0;
    for (i=0 ; i<numsprites ; i++)
    {
        if (!spritepresent[i])
            continue;

        for (j=0 ; j<sprites[i].numframes ; j++)
        {
            sf = &sprites[i].spriteframes[j];
            for (k=0 ; k<8 ; k++)
            {
            lump = firstspritelump + sf->lump[k];
            spritememory += lumpinfo[lump].size;
            W_CacheLumpNum(lump , PU_CACHE);
            }
        }
    }
}




