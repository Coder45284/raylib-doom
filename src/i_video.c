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
//	DOOM graphics stuff for raylib
//
//-----------------------------------------------------------------------------

#include "doomtype.h"

#include "raylib.h"

#include <stdlib.h>
#include <stdarg.h>

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

static Image display_image;

void I_ShutdownGraphics()
{
    UnloadImage( display_image );
    CloseWindow();
}

//
// I_StartFrame
//
void I_StartFrame()
{
    Color color = BLACK;

    for( d_int x = 0; x < SCREENHEIGHT; x++ ) {
        for( d_int y = 0; y < SCREENWIDTH; y++ ) {
            color.r = color.g = color.b = screens[0][ x * SCREENWIDTH + y ];

            ImageDrawPixel(&display_image, y, x, color);
        }
    }

    double span = (double)GetScreenHeight() / (double)SCREENHEIGHT;
    Vector2 position = {(GetScreenWidth() - span * SCREENWIDTH) / 2, 0};

    if( span * SCREENWIDTH > GetScreenWidth() ) {
        span = (double)GetScreenWidth() / (double)SCREENWIDTH;
        position.x = 0;
        position.y = (GetScreenHeight() - span * SCREENHEIGHT) / 2;
    }

    Texture2D display_texture = LoadTextureFromImage(display_image);

    BeginDrawing();

    ClearBackground(BLACK);

    DrawTextureEx(display_texture, position, 0.0f, span, WHITE);

    EndDrawing();

    UnloadTexture(display_texture);

}

void I_GetEvent()
{
}

//
// I_StartTic
//
void I_StartTic()
{
}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit()
{
    // what is this?
}

//
// I_FinishUpdate
//
void I_FinishUpdate()
{
}

//
// I_ReadScreen
//
void I_ReadScreen(byte* scr)
{
    memcpy (scr, screens[0], SCREENWIDTH * SCREENHEIGHT);
}

//
// I_SetPalette
//
void I_SetPalette(byte* palette)
{
}

void I_InitGraphics()
{
    display_image = GenImageColor(SCREENWIDTH, SCREENHEIGHT, PURPLE);

    int screen_width = SCREENWIDTH;
    int screen_height = SCREENHEIGHT;

    InitWindow(SCREENWIDTH, SCREENHEIGHT, "raylib doom");

    SetWindowState( FLAG_WINDOW_RESIZABLE );
    SetWindowMinSize(SCREENWIDTH, SCREENHEIGHT);
}
