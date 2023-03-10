@echo off

REM call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

call config.bat

@REM set CPP_VERSION=/std:c++14
set CPP_VERSION=/std:c++17
@REM set CPP_VERSION=/std:c++20
@REM set CPP_VERSION=/std:c++latest
@REM set CPP_VERSION=/std:c11
@REM set CPP_VERSION=/std:c17

set EXE_NAME=%PROJECT_NAME%
set EXE_NAME_DEBUG=%EXE_NAME%_DEBUG
set EXE_NAME_RELEASE=%EXE_NAME%_RELEASE

set INCLUDE_PATHS=
@REM set INCLUDE_PATHS=/I../vendor/include/GLEW

set LIB_PATHS=
@REM set LIB_PATHS=/LIBPATH:"../vendor/lib/GLEW/x64"
set LINK_LIBRARIES=user32.lib
@REM set LINK_LIBRARIES=user32.lib glew32.lib

@REM set INSTALL_DLL[0]=vendor\bin\GLEW\x64\glew32.dll

set SOURCE_PATH=../src/
set SOURCE_FILES=%SOURCE_PATH%main.cpp

mkdir build

    echo Copy dll files to build directory
    set "dllIterator=0"
    SETLOCAL EnableDelayedExpansion 
    :COPY_DLL_LOOP
    if defined INSTALL_DLL[%dllIterator%] (
        echo !INSTALL_DLL[%dllIterator%]! !cd!\
        copy !INSTALL_DLL[%dllIterator%]! !cd!\build\
        set /a "dllIterator+=1"
        GOTO :COPY_DLL_LOOP
    )

pushd build
cl /Fe"%EXE_NAME_DEBUG%" %CPP_VERSION% -Od -Zi -FC %INCLUDE_PATHS% %SOURCE_FILES% -link %LINK_LIBRARIES% %LIB_PATHS%
cl /Fe"%EXE_NAME_RELEASE%" %CPP_VERSION% -FC -Oix %INCLUDE_PATHS% %SOURCE_FILES% -link %LINK_LIBRARIES% %LIB_PATHS%
popd
