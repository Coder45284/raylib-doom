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

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

#define PALETTE_AMOUNT 0x100
static Color image_palette[PALETTE_AMOUNT];
static Image display_image;
static Texture2D display_texture;
static double screen_scale = 1.0;
static Vector2 screen_position = {0, 0};

static void adjust_scales() {
    screen_scale = (double)GetScreenHeight() / (double)SCREENHEIGHT;

    screen_position.x = (GetScreenWidth() - screen_scale * SCREENWIDTH) / 2;
    screen_position.y = 0;

    if( screen_scale * SCREENWIDTH > GetScreenWidth() ) {
        screen_scale = (double)GetScreenWidth() / (double)SCREENWIDTH;
        screen_position.x = 0;
        screen_position.y = (GetScreenHeight() - screen_scale * SCREENHEIGHT) / 2;
    }
}

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
}

void I_GetEvent()
{
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

    Vector2 mouse_delta = GetMouseDelta();
    int mouse_button_left = IsMouseButtonPressed( MOUSE_BUTTON_LEFT );
    int mouse_button_right = IsMouseButtonPressed( MOUSE_BUTTON_RIGHT );
    int mouse_button_middle = IsMouseButtonPressed( MOUSE_BUTTON_MIDDLE );

    if( mouse_delta.x != 0.0 || mouse_delta.y != 0.0 || // If mouse has moved
        mouse_button_left || mouse_button_right || mouse_button_middle )
    {
        event.type = ev_mouse;
        event.data1 = 0;
        event.data1 |= mouse_button_left   << 0;
        event.data1 |= mouse_button_right  << 1;
        event.data1 |= mouse_button_middle << 2;
        event.data2 = mouse_delta.x;
        event.data3 = -mouse_delta.y;
        D_PostEvent(&event);
    }


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

    KEYMAP( KEY_BACKSPACE, D_KEY_BACKSPACE )
    KEYMAP( KEY_PAUSE,     D_KEY_PAUSE )

    KEYMAP( KEY_EQUAL, D_KEY_EQUALS )
    KEYMAP( KEY_MINUS, D_KEY_MINUS )

    KEYMAP( KEY_RIGHT_SHIFT,   D_KEY_RSHIFT )
    KEYMAP( KEY_RIGHT_CONTROL, D_KEY_RCTRL )

    KEYMAP( KEY_RIGHT_ALT, D_KEY_RALT )
    KEYMAP( KEY_LEFT_ALT,  D_KEY_LALT )

    KEYMAP( KEY_LEFT_SHIFT,   D_KEY_RSHIFT )
    KEYMAP( KEY_LEFT_CONTROL, D_KEY_RCTRL )

    KEYMAP( KEY_APOSTROPHE, '\'' )
    KEYMAP( KEY_COMMA, ',' )
    KEYMAP( KEY_PERIOD, '.' )
    KEYMAP( KEY_SLASH, '/' )
    KEYMAP( KEY_ZERO, '0' )
    KEYMAP( KEY_ONE, '1' )
    KEYMAP( KEY_TWO, '2' )
    KEYMAP( KEY_THREE, '3' )
    KEYMAP( KEY_FOUR, '4' )
    KEYMAP( KEY_FIVE, '5' )
    KEYMAP( KEY_SIX, '6' )
    KEYMAP( KEY_SEVEN, '7' )
    KEYMAP( KEY_EIGHT, '8' )
    KEYMAP( KEY_NINE, '9' )
    KEYMAP( KEY_SEMICOLON, ';' )
    KEYMAP( KEY_A, 'a' )
    KEYMAP( KEY_B, 'b' )
    KEYMAP( KEY_C, 'c' )
    KEYMAP( KEY_D, 'd' )
    KEYMAP( KEY_E, 'e' )
    KEYMAP( KEY_F, 'f' )
    KEYMAP( KEY_G, 'g' )
    KEYMAP( KEY_H, 'h' )
    KEYMAP( KEY_I, 'i' )
    KEYMAP( KEY_J, 'j' )
    KEYMAP( KEY_K, 'k' )
    KEYMAP( KEY_L, 'l' )
    KEYMAP( KEY_M, 'm' )
    KEYMAP( KEY_N, 'n' )
    KEYMAP( KEY_O, 'o' )
    KEYMAP( KEY_P, 'p' )
    KEYMAP( KEY_Q, 'q' )
    KEYMAP( KEY_R, 'r' )
    KEYMAP( KEY_S, 's' )
    KEYMAP( KEY_T, 't' )
    KEYMAP( KEY_U, 'u' )
    KEYMAP( KEY_V, 'v' )
    KEYMAP( KEY_W, 'w' )
    KEYMAP( KEY_X, 'x' )
    KEYMAP( KEY_Y, 'y' )
    KEYMAP( KEY_Z, 'z' )
    KEYMAP( KEY_LEFT_BRACKET, '[' )
    KEYMAP( KEY_BACKSLASH, '\\' )
    KEYMAP( KEY_RIGHT_BRACKET, ']' )
    KEYMAP( KEY_GRAVE, '`' )
    KEYMAP( KEY_SPACE, ' ' )
    // Key: Tab
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

    for( d_int x = 0; x < SCREENHEIGHT; x++ ) {
        for( d_int y = 0; y < SCREENWIDTH; y++ ) {
            ImageDrawPixel(&display_image, y, x, image_palette[screens[0][ x * SCREENWIDTH + y ]]);
        }
    }

    UpdateTexture(display_texture, display_image.data);

    BeginDrawing();

    if(IsWindowResized()) {
        ClearBackground(BLACK);
        adjust_scales();
    }

    DrawTextureEx(display_texture, screen_position, 0.0f, screen_scale, WHITE);

    EndDrawing();
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
    byte* current_palette_color;

    for( d_short p = 0; p < PALETTE_AMOUNT; p++ ) {
        current_palette_color = &palette[ 3 * p ];

        image_palette[p].r = current_palette_color[ 0 ];
        image_palette[p].g = current_palette_color[ 1 ];
        image_palette[p].b = current_palette_color[ 2 ];
    }
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

    for( Color *p = image_palette; p < image_palette + PALETTE_AMOUNT; p++ ) {
        p->r = 0xFF;
        p->g = (p - image_palette);
        p->b = 0xFF;
        p->a = 0xFF;
    }

    adjust_scales();

    DisableCursor();
}
