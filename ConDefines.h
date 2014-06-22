/*
 * The MIT License (MIT)
 *
 * ConsoleDraw - Copyright (c) 2014, ZeroPage (kawoou@kawoou.kr)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#ifndef __CONSOLE_DRAW_DEFINES__
#define __CONSOLE_DRAW_DEFINES__

#define SOUND_BUFFER_COUNT	20

#define CONSOLE_W			80
#define CONSOLE_H			25
#define CONSOLE_SCROLL_POS	24

#define SAFE_FREE(p)		{if(p){free(p);(p)=NULL;}}

#define YES					1
#define NO					0

/* Mouse */
typedef enum MOUSE_BUTTON_ {
	MOUSE_BUTTON_LEFT = 0,
	MOUSE_BUTTON_MIDDLE,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_LCTRL,
	MOUSE_BUTTON_RCTRL,
	MOUSE_BUTTON_LSHIFT,
	MOUSE_BUTTON_RSHIFT,
	MOUSE_BUTTON_LALT,
	MOUSE_BUTTON_RALT,
	MOUSE_BUTTON_MAX
} MOUSE_BUTTON;

typedef struct {
	int x;
	int y;
} MOUSE_POINT;


/* Keyboard */
enum VIRTUAL_KEY {
	VK_0 = 0x30, VK_1, VK_2, VK_3, VK_4, VK_5, VK_6, VK_7, VK_8, VK_9,
	VK_A = 0x41, VK_B, VK_C, VK_D, VK_E, VK_F, VK_G, VK_H, VK_I, VK_J, VK_K, VK_L,
	VK_M, VK_N, VK_O, VK_P, VK_Q, VK_R, VK_S, VK_T, VK_U, VK_V, VK_W, VK_X, VK_Y, VK_Z
};
#define VK_RETURN		0x0D		// 'ENTER' KEY
#define VK_LSHIFT		0xA0		// SHIFT KEY.
#define VK_RSHIFT		0xA1
#define VK_LCONTROL		0xA2		// CONTROL KEY.
#define VK_RCONTROL		0xA3
#define VK_LMENU		0xA4		// MENU - ALT KEY.
#define VK_RMENU		0xA5 
#define VK_LALT			VK_LMENU
#define VK_RALT			VK_RMENU


/* Console */
typedef enum {
	CODEPAGE_KOR,
	CODEPAGE_ENG
} CODEPAGE;

typedef enum {
	READMODE_WAIT,
	READMODE_NOWAIT
} READMODE;

typedef enum {
	RECMODE_ERASE,
	RECMODE_NOERASE
} RECMODE;


// Screen Buffer
typedef struct {
	HANDLE						hCS;
	CONSOLE_SCREEN_BUFFER_INFO	CSBufInfo;
} CDSCREEN, *LPCDSCREEN;


// Screen Character Buffer
typedef struct {
	CHAR_INFO*					pBufArray;
	COORD						bufSize;
} CDCHAR, *LPCDCHAR;


// Input
typedef struct {
	HANDLE						hCI;
	INPUT_RECORD				InputRecord;
	COORD						MousePos;
} CDINPUT, *LPCDINPUT;


// Sound
typedef struct {
	HWND						hWnd;
	MCIDEVICEID					dev;
	UINT						soundID;
	UINT						times;
	BOOL						isPlaying;
} CDSOUND, *LPCDSOUND;

#endif /* __CONSOLE_DRAW_DEFINES__ */
