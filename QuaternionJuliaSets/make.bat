@echo off
setlocal
setlocal enableextensions

set NAME=QuaternionJuliaSets

if "%1" == "clean" if exist Common.pch del Common.pch

set CFLAGS=/Zi /I..\Common /nologo /W4 /EHa- /arch:AVX2 /wd4204 /wd4201
set DEBUG=/Od /D_DEBUG
set RELEASE=/O2 /fp:fast /DNDEBUG

if not defined CONFIG set CONFIG=%DEBUG%

if exist *.exe del *.exe

if not exist Common.pch (cl %CONFIG% %CFLAGS% /c /YcCommon.h ..\Common\Common.c)
if ERRORLEVEL 1 goto :end

cl %CONFIG% %CFLAGS% /YuCommon.h %NAME%.c /link Common.obj kernel32.lib user32.lib gdi32.lib /incremental:no /opt:ref
if ERRORLEVEL 1 goto :end

::cl %CONFIG% %CFLAGS% /YuCommon.h %NAME%AVX2.c /link Common.obj kernel32.lib user32.lib gdi32.lib /incremental:no /opt:ref
::if ERRORLEVEL 1 goto :end

:end
if exist %NAME%.obj del %NAME%.obj
if exist %NAME%AVX2.obj del %NAME%AVX2.obj
if "%1" == "run" if exist %NAME%.exe %NAME%.exe
