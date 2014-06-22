#include "ConDraw.h"

HINSTANCE	gInstance = NULL;
LPCDSOUND	gMusic = NULL;
LPCDSOUND	gSoundList[SOUND_BUFFER_COUNT] = { 0, };
char		gWinClassName[256] = {0, };

BYTE		gKeyValue[256] = {NO, };
BYTE		gKeyUpValue[256] = {YES, };
MOUSE_POINT	gMousePoint = {0, 0};
DWORD		gMouseValue[MOUSE_BUTTON_MAX] = {0, };

CDINPUT		gInput;
CDSCREEN	gScreen;
CDCHAR		gBuffers[2];
LPCDCHAR	gPriBuffer = NULL;
LPCDCHAR	gBackBuffer = NULL;

float		gFPS = 0.0f;
BOOL		gDrawableTargetScreen = YES;

#define CD_IsNotError(response, checkFlag, errorMsg)		\
{															\
	if (response == checkFlag)								\
	{														\
		fprintf(stderr, errorMsg);							\
		exit(1);											\
	}														\
}

void CD_Construct();
void CD_Create();
void CD_Delete();
BOOL CD_DrawBuffer();
void CD_CheckFPS();
void CD_GetMousePoint();
void CD_SetCodePage(CODEPAGE codePage);
void CD_UnloadSound(LPCDSOUND *hSound);
void CD_LoadSound(LPCDSOUND *hSound, const char *fileName);
void CD_PlaySound(LPCDSOUND hSound, int times);
void CD_PauseSound(LPCDSOUND hSound);
void CD_ResumeSound(LPCDSOUND hSound);
void CD_RewindSound(LPCDSOUND hSound);
void CD_StopSound(LPCDSOUND hSound);
unsigned int CD_Hash(const char *key);
LRESULT WINAPI SoundPlayProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

/* Public Methods */
// Console
void InitialConsole()
{
	CD_Construct();
	CD_Create();
}

void ReleaseConsole()
{
	CD_Delete();
	gPriBuffer = NULL;
	gBackBuffer = NULL;
}

void ClearScreen(WORD bkColor)
{
	int size;
	int i;
	CHAR_INFO temp;
	COORD sp = {0, 0};
	DWORD dwWritten;

	temp.Char.AsciiChar = ' ';
	temp.Attributes = CONSOLE_COLOR(BLACK, bkColor);

	if (gDrawableTargetScreen)
		FillConsoleOutputAttribute(gScreen.hCS, CONSOLE_COLOR(BLACK, bkColor), CONSOLE_W * CONSOLE_H, sp, &dwWritten);
	else
	{
		size = CONSOLE_W * CONSOLE_H;
		for (i = 0; i < size; i ++)
			gBackBuffer->pBufArray[i] = temp;
	}
}

float GetFPS()
{
	return gFPS;
}

WORD GetAttr(int x, int y)
{
	WORD attr = 0x00;

	if (x < 0 || x >= CONSOLE_W || y < 0 || y >= CONSOLE_H)
		return 0x00;

	if (gDrawableTargetScreen)
	{
		DWORD readnum = 0;
		COORD pos = {x, y};
		ReadConsoleOutputAttribute(gScreen.hCS, &attr, 1, pos, &readnum);
	}
	else
	{
		int pos = gBackBuffer->bufSize.X * y + x;
		attr = gBackBuffer->pBufArray[pos].Attributes;
	}

	return attr >> 4;
}

WORD GetChar(int x, int y)
{
	UCHAR charactor = 0x00;

	if (x < 0 || x >= CONSOLE_W || y < 0 || y >= CONSOLE_H)
		return 0x00;

	if (gDrawableTargetScreen)
	{
		DWORD readnum = 0;
		COORD pos = {x, y};
		ReadConsoleOutputCharacter(gScreen.hCS, (LPSTR)&charactor, 1, pos, &readnum);
	}
	else
	{
		int pos = gBackBuffer->bufSize.X * y + x;
		charactor = gBackBuffer->pBufArray[pos].Char.AsciiChar;
	}

	return charactor;
}

BOOL GetScreenNumber(int *pNum, int x, int y) 
{
	COORD pos = {x, y};
	SetConsoleCursorPosition(gScreen.hCS, pos);

	if(scanf("%d", pNum) == 0)
		return NO;

	_flushall();

	return YES;
}

int GetScreenString(char *pStr, int x, int y)
{
	COORD pos = {x, y};
    SetConsoleCursorPosition(gScreen.hCS, pos);

    gets(pStr);
     
    _flushall();

    return strlen(pStr);
}

void SetDrawableTarget(BOOL fromBuffer)
{
	gDrawableTargetScreen = !fromBuffer;
}

BOOL DrawChar(int x, int y, char c)
{
	return DrawChar(x, y, c, WHITE, BLACK);
}

BOOL DrawChar(int x, int y, char c, WORD color, WORD bkColor)
{
	BOOL res = YES;
	char msg[80];

	if (x < 0 || x >= CONSOLE_W || y < 0 || y >= CONSOLE_H)
	{
#ifdef _DEBUG
		sprintf(msg, "* DrawChar(): Out of bound error! [x:%d\ty:%d]\n", x, y);
		OutputDebugString(msg);
#endif
		return NO;
	}

	if (gDrawableTargetScreen)
	{
		COORD pos = {x, y};

		SetConsoleTextAttribute(gScreen.hCS, CONSOLE_COLOR(color, bkColor));
		res = SetConsoleCursorPosition(gScreen.hCS, pos);

		printf("%c", c);
	}
	else
	{
		int pos = gBackBuffer->bufSize.X * y + x;
		gBackBuffer->pBufArray[pos].Char.AsciiChar = c;
		gBackBuffer->pBufArray[pos].Attributes = CONSOLE_COLOR(color, bkColor);
	}

	return res;
}

BOOL DrawStr(int x, int y, const char *str)
{
	return DrawStr(x, y, str, WHITE, BLACK);
}

BOOL DrawStr(int x, int y, const char *str, WORD color, WORD bkColor)
{
	int i;
	BOOL res = YES;
	char msg[80] = {0,};

	int old_len = strlen(str);
	int new_len = old_len;

	if (x >= CONSOLE_W || y < 0 || y >= CONSOLE_H)
	{
#ifdef _DEBUG
		sprintf(msg, "* DrawStr(): Out of bound error! [x:%d\ty:%d]\n", x, y);
		OutputDebugString(msg);
#endif
		return NO;
	}

	if (x >= 0 && x < CONSOLE_W)
	{
		if (x + old_len > CONSOLE_W)
			new_len = CONSOLE_W - x;

		strncpy(msg, str, new_len);
	}
	else if (x < 0)
	{
		if (old_len <= abs(x))
			return NO;

		strcpy(msg, str + abs(x));
		x = 0;
	}

	if (gDrawableTargetScreen)
	{
		COORD pos = {x, y};

		SetConsoleTextAttribute(gScreen.hCS, CONSOLE_COLOR(color, bkColor));
		res = SetConsoleCursorPosition(gScreen.hCS, pos);

		printf("%s", msg);
	}
	else
	{
		int pos = gBackBuffer->bufSize.X * y + x;
		new_len = strlen(msg);

		for (i = 0; i < new_len; i ++)
		{
			gBackBuffer->pBufArray[pos + i].Char.AsciiChar = msg[i];
			gBackBuffer->pBufArray[pos + i].Attributes = CONSOLE_COLOR(color, bkColor);
		}
	}

	return res;
}

BOOL DrawHLine(int sx, int sy, int width, WORD bkColor)
{
	return DrawHCharLine(sx, sy, width, ' ', BLACK, bkColor);
}

BOOL DrawVLine(int sx, int sy, int height, WORD bkColor)
{
	return DrawVCharLine(sx, sy, height, ' ', BLACK, bkColor);
}

BOOL DrawRect(int sx, int sy, int width, int height, WORD bkColor)
{
	return DrawCharRect(sx, sy, width, height, ' ', BLACK, bkColor);
}

BOOL DrawHCharLine(int sx, int sy, int width, char c, WORD color, WORD bkColor)
{
	int i;
	int pos;
	COORD sp = {sx, sy};
	DWORD dwWritten;
	
	color = CONSOLE_COLOR(color, bkColor);

	if (gDrawableTargetScreen)
	{
		FillConsoleOutputAttribute(gScreen.hCS, color, width, sp, &dwWritten);
		FillConsoleOutputCharacter(gScreen.hCS, c, width, sp, &dwWritten);
	}
	else
	{
		pos = gBackBuffer->bufSize.X * sy + sx;
		for(i = 0; i < width; i ++, pos ++)
		{
			if (sx + i >= CONSOLE_W)
				break;

			gBackBuffer->pBufArray[pos].Char.AsciiChar = c;
			gBackBuffer->pBufArray[pos].Attributes = color;
		}
	}
	return YES;
}

BOOL DrawVCharLine(int sx, int sy, int height, char c, WORD color, WORD bkColor)
{
	int j;
	
	color = CONSOLE_COLOR(color, bkColor);

	if (gDrawableTargetScreen)
	{
		COORD pos;

		SetConsoleTextAttribute(gScreen.hCS, color);
		for(j = 0; j < height; j ++)
		{
			pos.X = sx;
			pos.Y = sy + j;
			SetConsoleCursorPosition(gScreen.hCS, pos);
			printf("%c", c);
		}
	}
	else
	{
		int pos;
		for(j = 0; j < height; j ++)
		{
			if (sx + j >= CONSOLE_H)
				break;

			pos = gBackBuffer->bufSize.X * (sy + j) + sx;
			gBackBuffer->pBufArray[pos].Char.AsciiChar = c;
			gBackBuffer->pBufArray[pos].Attributes = color;
		}
	}

	return YES;
}

BOOL DrawCharRect(int sx, int sy, int width, int height, char c, WORD color, WORD bkColor)
{
	BOOL ret = NO;
	ret |= DrawHCharLine(sx, sy, width, c, color, bkColor);
	ret |= DrawHCharLine(sx, sy + height - 1, width, c, color, bkColor);
	ret |= DrawVCharLine(sx, sy, height, c, color, bkColor);
	ret |= DrawVCharLine(sx + width - 1, sy, height, c, color, bkColor);

	return ret;
}

void Flip()
{
	static int flipcnt = 0;

	gBackBuffer = &gBuffers[flipcnt];
	gPriBuffer = &gBuffers[flipcnt^=1];

	CD_DrawBuffer();

	CD_CheckFPS();
}

void EngineSync(DWORD frm)
{
	static int frame = 0;
	static int sumtime = 0;
	static int oldtime = 0;
	int time = (1000.0f - sumtime) / (frm - (frame++));

	while (timeGetTime() - oldtime < time);
	if (oldtime != 0)
		sumtime += timeGetTime() - oldtime;
	oldtime = timeGetTime();

	if (sumtime >= 1000)
	{
		sumtime = sumtime % 1000;
		frame = 0;
	}
}

// Settings
void ChangeCodePage(BOOL isKorean)
{
#ifdef _DEBUG
	char msg[80];
#endif

	if(isKorean) 
#ifdef _DEBUG
	{
		sprintf(msg, "* Change Code Page. 949\n");
#endif
		CD_SetCodePage(CODEPAGE_KOR);
#ifdef _DEBUG
	}
#endif
	else
#ifdef _DEBUG
	{
		sprintf(msg, "* Change Code Page. 437\n");
#endif
		CD_SetCodePage(CODEPAGE_ENG);
#ifdef _DEBUG
	}

	OutputDebugString(msg);
#endif
}

void SetWindowTitle(const char *title)
{
	SetConsoleTitle(title);
}

char *GetWindowTitle()
{
	char buffer[512] = {0,};
	if (!GetConsoleTitle(buffer, 512))
		return NULL;

	return buffer;
}

// Input
void CheckInput()
{
	DWORD i;
	DWORD dwUnread = 0;
	DWORD dwRead = 0;
	WORD keycode;
	BOOL bkeydown;

	GetNumberOfConsoleInputEvents(gInput.hCI, &dwUnread);
	if (dwUnread == 0)
		return;

	memset(&(gInput.InputRecord), 0, sizeof(INPUT_RECORD));

	for (i = 0; i < dwUnread; i ++)
	{
		ReadConsoleInput(gInput.hCI, &gInput.InputRecord, 1, &dwRead);

		switch (gInput.InputRecord.EventType)
		{
			case KEY_EVENT:
				keycode = gInput.InputRecord.Event.KeyEvent.wVirtualKeyCode;
				bkeydown = gInput.InputRecord.Event.KeyEvent.bKeyDown;

				gKeyValue[keycode] = bkeydown;

				if (bkeydown == YES)
					gKeyUpValue[keycode] = NO;

				break;

			case MOUSE_EVENT:
				memset(gMouseValue, 0, sizeof(gMouseValue));

				switch (gInput.InputRecord.Event.MouseEvent.dwButtonState)
				{
					case FROM_LEFT_1ST_BUTTON_PRESSED:
						gMouseValue[MOUSE_BUTTON_LEFT] = 1;
						break;

					case RIGHTMOST_BUTTON_PRESSED:
						gMouseValue[MB_RIGHT] = 1;
						break;

					case FROM_LEFT_2ND_BUTTON_PRESSED:
						gMouseValue[MOUSE_BUTTON_MIDDLE] = 1;
						break;
				}
				CD_GetMousePoint();

				break;

			case FOCUS_EVENT:
				break;
		}
	}

	for (i = 0xa0; i < 0xa6; i ++)
	{
		gKeyValue[i] = GetAsyncKeyState(i) ? YES : NO;

		if(gKeyValue[i] == YES)
			gKeyUpValue[i] = NO;
	}

	FlushConsoleInputBuffer(gInput.hCI);
}

void ClearInputData()
{
	memset(gKeyValue, 0, sizeof(gKeyValue));
	memset(gKeyUpValue, YES, sizeof(gKeyUpValue));
	memset(gMouseValue, 0, sizeof(gMouseValue));  
	gMousePoint.x = 0; 
	gMousePoint.y = 0; 
}

BOOL IsAnyKey()
{
	int i;
	for (i = 0; i < 256; i ++)
	{
		if (gKeyValue[i])
			return YES; 
	}

	return NO;
}

BOOL IsKey(int vKey)
{
	return (gKeyValue[vKey] == YES);
}

BOOL IsKeyUp(int vKey)
{
	if (gKeyValue[vKey] == NO && gKeyValue[vKey] == NO)    
	{
		return (gKeyValue[vKey] = YES);
	}
	
	return NO;
}

BOOL IsMouseDown(int key)
{
	return (gMouseValue[key] == YES);
}

// Sound
void PlayMusic(const char *fileName, BOOL isLoop)
{
	CD_UnloadSound(&gMusic);

	CD_LoadSound(&gMusic, fileName);
	CD_PlaySound(gMusic, isLoop?-1:1);
}

BOOL IsPlayMusic()
{
	if (gMusic == NULL)
		return NO;

	return gMusic->isPlaying;
}

void PauseMusic()
{
	CD_PauseSound(gMusic);
}

void ResumeMusic()
{
	CD_ResumeSound(gMusic);
}

void RewindMusic()
{
	CD_RewindSound(gMusic);
}

void StopMusic()
{
	CD_UnloadSound(&gMusic);
}

LPCDSOUND PlaySound(const char *fileName, int playCount)
{
	int i;

	if (playCount == 0)
		return NULL;

	for (i = 0; i < SOUND_BUFFER_COUNT; i ++)
	{
		if (gSoundList[i] == NULL)
		{
			CD_LoadSound(gSoundList + i, fileName);
			CD_PlaySound(gSoundList[i], playCount);

			return gSoundList[i];
		}
	}

	return NULL;
}

BOOL IsPlaySound(LPCDSOUND hSound)
{
	if (hSound == NULL)
		return NO;

	return hSound->isPlaying;
}

void PauseSound(LPCDSOUND hSound)
{
	CD_PauseSound(hSound);
}

void ResumeSound(LPCDSOUND hSound)
{
	CD_ResumeSound(hSound);
}

void RewindSound(LPCDSOUND hSound)
{
	CD_RewindSound(hSound);
}

void StopSound(LPCDSOUND *hSound)
{
	CD_UnloadSound(hSound);
}

void StopAllSound()
{
	int i;
	for (i = 0; i < SOUND_BUFFER_COUNT; i ++)
	{
		if (gSoundList[i])
			CD_UnloadSound(gSoundList + i);
	}
}

/* Private Methods */
void CD_Construct()
{
	int i;

	gMusic = NULL;
	memset(gSoundList, 0, sizeof(LPCDCHAR) * 20);

	memset(&gScreen, 0, sizeof(CDSCREEN));
	memset(&gInput, 0, sizeof(CDINPUT));
	for (i = 0; i < 2; i ++)
		memset(gBuffers + i, 0, sizeof(CDCHAR));

	gBackBuffer = gBuffers + 1;
	gPriBuffer  = gBuffers + 0;

	memset(gKeyValue, NO, sizeof(gKeyValue));
	memset(gKeyUpValue, YES, sizeof(gKeyUpValue));
	memset(gMouseValue, NO, sizeof(gMouseValue));
	gMousePoint.x = -1;
	gMousePoint.y = -1;

	gDrawableTargetScreen = YES;
}

void CD_Create()
{
	int i;

	CD_SetCodePage(CODEPAGE_KOR);

	// Get handle
	gScreen.hCS = GetStdHandle(STD_OUTPUT_HANDLE);
	gInput.hCI = GetStdHandle(STD_INPUT_HANDLE);

	// Error check
	CD_IsNotError(gScreen.hCS, INVALID_HANDLE_VALUE, "CD_Create(): Invalid handle, gScreen!");
	CD_IsNotError(gInput.hCI, INVALID_HANDLE_VALUE, "CD_Create(): Invalid handle, gInput!");

	// Set screen buffer
	COORD ScreenSize = {CONSOLE_W, CONSOLE_H};
	SetConsoleScreenBufferSize(gScreen.hCS, ScreenSize);
	GetConsoleScreenBufferInfo(gScreen.hCS, &(gScreen.CSBufInfo));

	// Hide cursor
	CONSOLE_CURSOR_INFO CurInfo;
	CurInfo.dwSize = 100;
	CurInfo.bVisible = FALSE;
	SetConsoleCursorInfo(gScreen.hCS, &CurInfo);

	// Enable mouse input
	DWORD modeOld;
	DWORD modeNew;
	GetConsoleMode(gInput.hCI, &modeOld);
	modeNew = modeOld;
	modeNew = modeNew | ENABLE_MOUSE_INPUT;

	CD_IsNotError(SetConsoleMode(gInput.hCI, modeNew), FALSE, "CD_Create(): SetConsoleMode() Error!");

	for (i = 0; i < 2; i ++)
	{
		gBuffers[i].bufSize.X = CONSOLE_W;
	    gBuffers[i].bufSize.Y = CONSOLE_H;

	    gBuffers[i].pBufArray = (CHAR_INFO *)malloc(CONSOLE_W * CONSOLE_H * sizeof(CHAR_INFO));

	    CD_IsNotError(gBuffers[i].pBufArray, NULL, "CD_Create(): Null Pointer Exception, gBuffers[i]");
	    
	    memset(gBuffers[i].pBufArray, 0, CONSOLE_W * CONSOLE_H * sizeof(CHAR_INFO));
	}
}

void CD_Delete()
{
	int i;

	memset(&gScreen, 0, sizeof(CDSCREEN));
	memset(&gInput, 0, sizeof(CDINPUT));
	for (i = 0; i < 2; i ++)
	{
		SAFE_FREE(gBuffers[i].pBufArray);
		memset(gBuffers + i, 0, sizeof(CDCHAR));
	}
}

BOOL CD_DrawBuffer()
{
	COORD pos = {0, 0};
	SMALL_RECT rc = {0, 0, CONSOLE_W - 1, CONSOLE_H - 1};

	if (gDrawableTargetScreen)
		return WriteConsoleOutput(gScreen.hCS, gPriBuffer->pBufArray, gPriBuffer->bufSize, pos, &rc);

	return YES;
}

void CD_CheckFPS()
{
	static int oldtime = 0;
	static int frmcnt = 0;
	static float fps = .0;

	frmcnt++;
	int time = timeGetTime() - oldtime;
	if (time > 1000)
	{
		oldtime = timeGetTime();
		fps = (float)((float)(frmcnt * 1000) / (float)time);
		frmcnt = 0;
	}

	gFPS = fps;
}

void CD_SetCodePage(CODEPAGE codePage)
{
	switch (codePage)
	{
		case CODEPAGE_KOR:
			system("chcp 949");
			break;

		default:
			system("chcp 437");
	}
}

void CD_GetMousePoint()
{
	gMousePoint.x = gInput.InputRecord.Event.MouseEvent.dwMousePosition.X;
    gMousePoint.y = gInput.InputRecord.Event.MouseEvent.dwMousePosition.Y;
}

void CD_UnloadSound(LPCDSOUND *hSound)
{
	int i;
	if (hSound == NULL)
		return;

	if ((*hSound) == NULL)
		return;

	if ((*hSound)->isPlaying)
		CD_StopSound(*hSound);
	
	if ((*hSound)->dev)
		mciSendCommand((*hSound)->dev, MCI_CLOSE, 0, 0);

	if ((*hSound)->hWnd)
		DestroyWindow((*hSound)->hWnd);

	(*hSound)->dev = 0;
	(*hSound)->isPlaying = NO;
	(*hSound)->hWnd = NULL;

	for (i = 0; i < SOUND_BUFFER_COUNT; i ++)
	{
		if (gSoundList[i] == *hSound)
		{
			gSoundList[i] = NULL;
			break;
		}
	}

	*hSound = NULL;
}

void CD_LoadSound(LPCDSOUND *hSound, const char *fileName)
{
	int nLen;
	CDSOUND soundPlayer;
	MCIERROR mciError;
	MCI_OPEN_PARMS mciOpen = {0};

	if (hSound == NULL)
		return;

	if (fileName == NULL)
		return;

	nLen = (int)strlen(fileName);
	if (nLen == 0)
		return;

	if (!gInstance)
	{
		gInstance = GetModuleHandle(NULL);

		sprintf(gWinClassName, "ConsoleDraw_%d", time(NULL));

		WNDCLASS wc;
		wc.style			= 0;
		wc.lpfnWndProc		= SoundPlayProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= gInstance;
		wc.hIcon			= 0;
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground	= NULL;
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= gWinClassName;

		if (!RegisterClass(&wc) &&
			1410 != GetLastError())
			return;
	}

	soundPlayer.hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		gWinClassName,
		NULL,
		WS_POPUPWINDOW,
		0,
		0,
		0,
		0,
		NULL,
		NULL,
		gInstance,
		NULL);

	if (soundPlayer.hWnd)
	{
		SetWindowLongPtr(soundPlayer.hWnd, GWLP_USERDATA, (LONG_PTR)&soundPlayer);

		soundPlayer.dev = 0;
		soundPlayer.isPlaying = NO;

		mciOpen.lpstrDeviceType = (LPCTSTR)MCI_ALL_DEVICE_ID;
		mciOpen.lpstrElementName = fileName;
		
		mciError = mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD_PTR)(&mciOpen));
		if (mciError)
			return;

		soundPlayer.dev = mciOpen.wDeviceID;
		soundPlayer.soundID = CD_Hash(fileName);
		soundPlayer.isPlaying = 0;

		*hSound = &soundPlayer;
	}
}

void CD_PlaySound(LPCDSOUND hSound, int times)
{
	MCIERROR mciError;
	MCI_PLAY_PARMS mciPlay = {0};

	if (hSound == NULL)
		return;

	if (hSound->dev == NULL)
		return;

	mciPlay.dwCallback = (DWORD_PTR)hSound->hWnd;
	mciError = mciSendCommand(hSound->dev, MCI_PLAY, MCI_FROM | MCI_NOTIFY, (DWORD_PTR)(&mciPlay));
	if (mciError == NULL)
	{
		hSound->isPlaying = YES;
		hSound->times = times;
	}
}

void CD_PauseSound(LPCDSOUND hSound)
{
	if (hSound == NULL)
		return;
	
	if (hSound->dev)
		mciSendCommand(hSound->dev, MCI_PAUSE, 0, 0);
}

void CD_ResumeSound(LPCDSOUND hSound)
{
	if (hSound == NULL)
		return;

	if (hSound->dev)
		mciSendCommand(hSound->dev, MCI_RESUME, 0, 0);
}

void CD_RewindSound(LPCDSOUND hSound)
{
	MCI_PLAY_PARMS mciPlay = {0};

	if (hSound == NULL)
		return;
	
	if (hSound->dev == NULL)
		return;

	mciPlay.dwCallback = (DWORD_PTR)hSound->hWnd;
	hSound->isPlaying = mciSendCommand(hSound->dev, MCI_PLAY, MCI_NOTIFY, (DWORD_PTR)(&mciPlay)) ? NO : YES;
}

void CD_StopSound(LPCDSOUND hSound)
{
	if (hSound == NULL)
		return;

	if (hSound->dev == NULL)
		return;

	mciSendCommand(hSound->dev, MCI_STOP, 0, 0);
	hSound->isPlaying = NO;
}

unsigned int CD_Hash(const char *key)
{
	unsigned int len = strlen(key);
	const char *end = key + len;
	unsigned int hash;

	for (hash = 0; key < end; key ++)
	{
		hash *= 16777619;
		hash ^= (unsigned int)(unsigned char)toupper(*key);
	}
	return hash;
}

/* Handler */
LRESULT WINAPI SoundPlayProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	LPCDSOUND player = NULL;
	MCI_PLAY_PARMS mciPlay = {0};

	if (MM_MCINOTIFY == Msg && MCI_NOTIFY_SUCCESSFUL == wParam &&
		(player = (LPCDSOUND)GetWindowLongPtr(hWnd, GWLP_USERDATA)))
	{
		if(player->times > 0)
			--player->times;
		
		if(player->times != 0)
		{
			mciSendCommand(lParam, MCI_SEEK, MCI_SEEK_TO_START, 0);

			mciPlay.dwCallback = (DWORD_PTR)hWnd;
			mciSendCommand(lParam, MCI_PLAY, MCI_NOTIFY, (DWORD_PTR)(&mciPlay));
		}
		else
			CD_UnloadSound(&player);

		return 0;
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}