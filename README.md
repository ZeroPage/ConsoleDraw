ConsoleDraw
===========

Drawing on win32 console environment

##How-To
####Initalize
``` c
InitialConsole();                     // Intialize
```

####Release
``` c
ReleaseConsole();                     // Release
```

####Drawable Target
``` c
...

SetDrawableTarget(true);              // Drawable target is back-buffer

ClearScreen(BLACK);
...
/* Drawing on back-buffer */
...

SetDrawableTarget(false);             // Drawable target is screen

Flip();                               // Screen = Back-buffer

...
```

####Input
``` c
CheckInput();                         // Update input data

...

if (IsAnyKey())                       // Any input at keyboard
  ...

if (IsKey(VK_A))                      // Specific input at keyboard
  ...
  
if (IsKeyUp(VK_B))                    // KeyUp event occur once,
  ...                                 // immediately after key release

if (IsMouseDown(MOUSE_BUTTON_LEFT))   // Specific input at Mouse
  ...

```

####Play Sound
``` c
PlayMusic("background.mp3", false);
if (IsPlayMusic())
  ...
PauseMusic();
ResumeMusic();
RewindMusic();
StopMusic();                          // Auto free

...

LPCDSOUND hSound = PlaySound("effect.mp3", -1);
if (IsPlaySound(hSound))
  ...
PauseSound(hSound);
ResumeSound(hSound);
RewindSound(hSound);
StopSound(&hSound);                   // Auto free

...

StopAllSound();         // Not music
```

<br />

##Requirements

- Windows XP / Vista / 7 / 8
- Not support, MacOS or Linux

<br />

## Changelog

+ 1.0 ConsoleDraw Library

<br />

##License

> The MIT License (MIT)
>
>  ConsoleDraw - Copyright (c) 2014, ZeroPage (kawoou@kawoou.kr)
>
>  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following condi tions:
>
>  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
>  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
