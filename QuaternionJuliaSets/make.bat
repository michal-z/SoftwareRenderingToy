@echo off
setlocal
setlocal enableextensions

set NAME=QuaternionJuliaSets

if "%1" == "clean" if exist Common.pch del Common.pch

set CFLAGS=/Zi /I..\Common /nologo /W4 /EHa- /arch:AVX2 /wd4204 /wd4201

set CONFIG=/O2 /fp:fast /DNDEBUG
::set CONFIG=/Od /D_DEBUG

if exist %NAME%.exe del %NAME%.exe

if not exist Common.pch (cl %CONFIG% %CFLAGS% /c /YcCommon.h ..\Common\Common.c)
if ERRORLEVEL 1 goto :end

cl %CONFIG% %CFLAGS% /YuCommon.h %NAME%.c /link Common.obj kernel32.lib user32.lib gdi32.lib /incremental:no /opt:ref

:end
if exist %NAME%.obj del %NAME%.obj
if "%1" == "run" if exist %NAME%.exe %NAME%.exe
