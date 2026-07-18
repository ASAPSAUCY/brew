@echo off
setlocal EnableDelayedExpansion
title Memory Quest - build and play
cd /d "%~dp0"

set "PROJECT=%~dp0MemoryQuest.uproject"
if not exist "%PROJECT%" (
    echo Could not find MemoryQuest.uproject next to this script.
    echo Keep PlayMe.bat inside the memory-quest folder and run it from there.
    pause
    exit /b 1
)

echo ============================================
echo   MEMORY QUEST - Kyle ^& Claudia
echo   One-click build and play
echo ============================================
echo.

rem ---- Find Unreal Engine -------------------------------------------------
set "ENGINE="
for /f "tokens=2*" %%A in ('reg query "HKLM\SOFTWARE\EpicGames\Unreal Engine\5.8" /v InstalledDirectory 2^>nul ^| findstr /i InstalledDirectory') do set "ENGINE=%%B"
if defined ENGINE if not exist "!ENGINE!\Engine\Build\BatchFiles\Build.bat" set "ENGINE="
if not defined ENGINE (
    for /d %%D in ("%ProgramFiles%\Epic Games\UE_*") do (
        if exist "%%~fD\Engine\Build\BatchFiles\Build.bat" set "ENGINE=%%~fD"
    )
)
if not defined ENGINE (
    echo Unreal Engine was NOT found on this PC.
    echo.
    echo Fix: open the Epic Games Launcher, go to the Unreal Engine tab,
    echo and install Unreal Engine 5.8. Then run this script again.
    pause
    exit /b 1
)
echo Found Unreal Engine:  !ENGINE!

rem ---- Check Visual Studio C++ tools --------------------------------------
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSPATH="
if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%V in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSPATH=%%V"
)
if not defined VSPATH (
    echo Visual Studio 2022 with C++ tools was NOT found - the game cannot
    echo compile without it. This is almost certainly why the build failed.
    echo.
    echo Fix - takes about 15 minutes:
    echo   1. Install "Visual Studio 2022 Community" - it is free
    echo   2. In its installer, tick the workload "Game development with C++"
    echo   3. Run this script again
    echo.
    echo Opening the download page for you now...
    start "" "https://visualstudio.microsoft.com/vs/community/"
    pause
    exit /b 1
)
echo Found Visual Studio:  !VSPATH!

rem ---- Build ---------------------------------------------------------------
echo.
echo Building Memory Quest... first time takes a few minutes. Coffee time.
echo.
call "!ENGINE!\Engine\Build\BatchFiles\Build.bat" MemoryQuestEditor Win64 Development -project="%PROJECT%" -waitmutex
if errorlevel 1 (
    echo.
    echo ============================================
    echo   BUILD FAILED
    echo ============================================
    echo Scroll up to the first line containing the word "error",
    echo copy that whole section, and send it to Claude to fix.
    pause
    exit /b 1
)

rem ---- Play ----------------------------------------------------------------
echo.
echo Build OK! Launching the Unreal Editor - press the Play button when
echo it opens. First launch also compiles shaders, so give it a minute.
start "" "!ENGINE!\Engine\Binaries\Win64\UnrealEditor.exe" "%PROJECT%"
exit /b 0
