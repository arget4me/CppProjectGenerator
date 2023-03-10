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

set LINK_LIBRARIES=user32.lib

set SOURCE_PATH=../src/
set SOURCE_FILES=%SOURCE_PATH%main.cpp

mkdir build
pushd build
cl /Fe"%EXE_NAME_DEBUG%" %CPP_VERSION% -Od -Zi -FC %SOURCE_FILES% -link %LINK_LIBRARIES%
cl /Fe"%EXE_NAME_RELEASE%" %CPP_VERSION% -FC -Oix %SOURCE_FILES% -link %LINK_LIBRARIES%
popd
