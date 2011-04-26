@echo off
rem ==========================================================================
rem Product: QSPY buld script for Win32 port with GNU (MinGW)
rem Last Updated for Version: 4.1.06
rem Date of the Last Update:  Feb 09, 2011
rem
rem                    Q u a n t u m     L e a P s
rem                    ---------------------------
rem                    innovating embedded systems
rem
rem Copyright (C) 2002-2011 Quantum Leaps, LLC. All rights reserved.
rem
rem This software may be distributed and modified under the terms of the GNU
rem General Public License version 2 (GPL) as published by the Free Software
rem Foundation and appearing in the file GPL.TXT included in the packaging of
rem this file. Please note that GPL Section 2[b] requires that all works based
rem on this software must also be made publicly available under the terms of
rem the GPL ("Copyleft").
rem
rem Alternatively, this software may be distributed and modified under the
rem terms of Quantum Leaps commercial licenses, which expressly supersede
rem the GPL and are specifically designed for licensees interested in
rem retaining the proprietary status of their code.
rem
rem Contact information:
rem Quantum Leaps Web site:  http://www.quantum-leaps.com
rem e-mail:                  info@quantum-leaps.com
rem ==========================================================================
setlocal

rem NOTE:
rem -----
rem The following two symbols define the location of the compiler and the
rem archiver. 

set MINGW=C:\tools\MinGW

set CC=%MINGW%\bin\g++
set LINK=%MINGW%\bin\g++
set LIBDIR=%MINGW%\lib

rem ==========================================================================
set QP_INCDIR=..\..\..\..\include
set QP_PRTDIR=.

if "%1"=="dbg" (
    echo default selected
    set BINDIR=%QP_PRTDIR%\dbg
    set CCFLAGS=-g -c -Wall 
)
if "%1"=="" (
    echo rel selected
    set BINDIR=%QP_PRTDIR%\rel
    set CCFLAGS=-O -c -Wall 
)

set LINKFLAGS=

rem QSPY ---------------------------------------------------------------------
set SRCDIR=..\..\source
set CCINC=-I%QP_INCDIR% -I..\..\include

@echo on
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\dict.o     %SRCDIR%\dict.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\getopt.o   %SRCDIR%\getopt.c
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\main.o     %SRCDIR%\main.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qspy.o     %SRCDIR%\qspy.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\com.o      .\com.cpp     
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\tcp.o      .\tcp.cpp

%LINK% %LINKFLAGS% -o %BINDIR%\qspy.exe %BINDIR%\dict.o %BINDIR%\getopt.o %BINDIR%\main.o %BINDIR%\qspy.o %BINDIR%\com.o %BINDIR%\tcp.o %LIBDIR%\libwsock32.a
@echo off
erase %BINDIR%\*.o

:end


endlocal