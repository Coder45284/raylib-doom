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
static Texture2D display_texture;

void I_ShutdownGraphics()
{
    UnloadImage(display_image);
    UnloadTexture(display_texture);
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

    UpdateTexture(display_texture, display_image.data);

    BeginDrawing();

    ClearBackground(BLACK);

    DrawTextureEx(display_texture, position, 0.0f, span, WHITE);

    EndDrawing();

}

static bool i_events_remaining;

void I_GetEvent()
{
    event_t event;

    int keycode_pressed = GetKeyPressed();

    if(keycode_pressed == KEY_NULL) {
        i_events_remaining = false;
    }
    else
        printf( "Keycode = %i\n", keycode_pressed );
}

#define KEYMAP( RL_KEY, D_KEY )\
if( IsKeyPressed( RL_KEY ) ) {\
    event.type = ev_keydown;\
    event.data1 = D_KEY;\
    D_PostEvent(&event);\
}\
if( IsKeyReleased( RL_KEY ) ) {\
    event.type = ev_keyup;\
    event.data1 = D_KEY;\
    D_PostEvent(&event);}
//
// I_StartTic
//
void I_StartTic()
{
    event_t event;

    if( WindowShouldClose() ) {
        I_Quit();
    }

    //i_events_remaining = true;
    //while( i_events_remaining )
        //I_GetEvent();

    KEYMAP( KEY_RIGHT,  D_KEY_RIGHTARROW )
    KEYMAP( KEY_LEFT,   D_KEY_LEFTARROW )
    KEYMAP( KEY_UP,     D_KEY_UPARROW )
    KEYMAP( KEY_DOWN,   D_KEY_DOWNARROW )
    KEYMAP( KEY_ESCAPE, D_KEY_ESCAPE )
    KEYMAP( KEY_ENTER,  D_KEY_ENTER )
    KEYMAP( KEY_TAB,    D_KEY_TAB )
    KEYMAP( KEY_F1,     D_KEY_F1 )
    KEYMAP( KEY_F2,     D_KEY_F2 )
    KEYMAP( KEY_F3,     D_KEY_F3 )
    KEYMAP( KEY_F4,     D_KEY_F4 )
    KEYMAP( KEY_F5,     D_KEY_F5 )
    KEYMAP( KEY_F6,     D_KEY_F6 )
    KEYMAP( KEY_F7,     D_KEY_F7 )
    KEYMAP( KEY_F8,     D_KEY_F8 )
    KEYMAP( KEY_F9,     D_KEY_F9 )
    KEYMAP( KEY_F10,    D_KEY_F10 )
    KEYMAP( KEY_F11,    D_KEY_F11 )
    KEYMAP( KEY_F12,    D_KEY_F12 )
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
    int screen_width = SCREENWIDTH;
    int screen_height = SCREENHEIGHT;

    InitWindow(screen_width, screen_height, "raylib doom");

    SetExitKey(0);

    if( GetMonitorCount() != 0 ) {
        const int current_monitor = GetCurrentMonitor();
        const int monitor_width   = GetMonitorWidth(  current_monitor );
        const int monitor_height  = GetMonitorHeight( current_monitor );

        while( monitor_width > 2 * screen_width && monitor_height > 2 * screen_height ) {
            screen_width  *= 2;
            screen_height *= 2;
        }

        SetWindowSize( screen_width, screen_height );
        SetWindowPosition(
            (monitor_width  - screen_width)  / 2,
            (monitor_height - screen_height) / 2 );
    }

    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(SCREENWIDTH, SCREENHEIGHT);

    display_image = GenImageColor(SCREENWIDTH, SCREENHEIGHT, PURPLE);
    display_texture = LoadTextureFromImage(display_image);
}
