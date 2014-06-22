#pragma once

#ifndef __CONSOLE_DRAW__
#define __CONSOLE_DRAW__

#pragma warning (disable:4996)

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <memory.h>
#include <process.h>
#include <time.h>

#include "ConColors.h"
#include "ConDefines.h"

#include "mmsystem.h"    
#pragma comment(lib, "winmm.lib")

/* Input */
void CheckInput();
void ClearInputData();

BOOL IsAnyKey();
BOOL IsKey(int vKey);
BOOL IsKeyUp(int vKey);
BOOL IsMouseDown(int key);


/* Console */
void InitialConsole();
void ReleaseConsole();

void ClearScreen(WORD bkColor);
float GetFPS();

WORD GetAttr(int x, int y);
WORD GetChar(int x, int y);
BOOL GetScreenNumber(int *pNum, int x, int y);
int GetScreenString(char *pStr, int x, int y);

void SetDrawableTarget(BOOL fromBuffer);

BOOL DrawChar(int x, int y, char c);
BOOL DrawChar(int x, int y, char c, WORD color, WORD bkColor);
BOOL DrawStr(int x, int y, const char *str);
BOOL DrawStr(int x, int y, const char *str, WORD color, WORD bkColor);

BOOL DrawHLine(int sx, int sy, int width, WORD bkColor);
BOOL DrawVLine(int sx, int sy, int height, WORD bkColor);
BOOL DrawRect(int sx, int sy, int width, int height, WORD bkColor);

BOOL DrawHCharLine(int sx, int sy, int width, char c, WORD color, WORD bkColor);
BOOL DrawVCharLine(int sx, int sy, int height, char c, WORD color, WORD bkColor);
BOOL DrawCharRect(int sx, int sy, int width, int height, char c, WORD color, WORD bkColor);

void Flip();
void EngineSync(DWORD frm);


/* Settings */
void ChangeCodePage(BOOL isKorean);
void SetWindowTitle(const char *title);
char *GetWindowTitle();


/* Sound */
void PlayMusic(const char *fileName, BOOL isLoop);
BOOL IsPlayMusic();
void PauseMusic();
void ResumeMusic();
void RewindMusic();
void StopMusic();

LPCDSOUND PlaySound(const char *fileName, int playCount);	// -1: infinity loop
BOOL IsPlaySound(LPCDSOUND hSound);
void PauseSound(LPCDSOUND hSound);
void ResumeSound(LPCDSOUND hSound);
void RewindSound(LPCDSOUND hSound);
void StopSound(LPCDSOUND *hSound);
void StopAllSound();


#endif /* __CONSOLE_DRAW__ */
