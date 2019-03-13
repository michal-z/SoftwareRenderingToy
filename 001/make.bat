@echo off
setlocal
setlocal enableextensions

set NAME=001

if "%1" == "clean" if exist Common.pch del Common.pch

::set CFLAGS=/Zi /O2 /DNDEBUG /I..\Common /nologo /W4 /EHa-
set CFLAGS=/Zi /Od /D_DEBUG /I..\Common /nologo /W4 /EHa-

if exist %NAME%.exe del %NAME%.exe

if not exist Common.pch (cl %CFLAGS% /c /YcCommon.h ..\Common\Common.c)
if ERRORLEVEL 1 goto :end

cl %CFLAGS% /YuCommon.h %NAME%.c /link Common.obj kernel32.lib user32.lib gdi32.lib /incremental:no /opt:ref

:end
if exist %NAME%.obj del %NAME%.obj
if "%1" == "run" if exist %NAME%.exe %NAME%.exe
