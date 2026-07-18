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

rem ---- Find Unreal Engine on ANY drive ------------------------------------
set "ENGINE="

rem 0. A previously remembered location.
if exist "%~dp0EnginePath.txt" (
    set /p ENGINE=<"%~dp0EnginePath.txt"
    if defined ENGINE if not exist "!ENGINE!\Engine\Build\BatchFiles\Build.bat" set "ENGINE="
)

rem 1. The Windows registry - Epic records every engine install here,
rem    whichever drive it lives on.
if not defined ENGINE (
    for /f "delims=" %%K in ('reg query "HKLM\SOFTWARE\EpicGames\Unreal Engine" 2^>nul') do (
        for /f "tokens=2*" %%A in ('reg query "%%K" /v InstalledDirectory 2^>nul ^| findstr /i InstalledDirectory') do (
            if exist "%%B\Engine\Build\BatchFiles\Build.bat" set "ENGINE=%%B"
        )
    )
)

rem 2. The Epic Games Launcher's own install list, also drive-independent.
if not defined ENGINE (
    for /f "usebackq delims=" %%P in (`powershell -NoProfile -ExecutionPolicy Bypass -Command "$d = Join-Path $env:ProgramData 'Epic\UnrealEngineLauncher\LauncherInstalled.dat'; if (Test-Path $d) { (Get-Content -Raw $d | ConvertFrom-Json).InstallationList | ForEach-Object InstallLocation }" 2^>nul`) do (
        if exist "%%P\Engine\Build\BatchFiles\Build.bat" set "ENGINE=%%P"
    )
)

rem 3. Scan the usual folder names on every drive letter.
if not defined ENGINE (
    for %%L in (C D E F G H I J K) do (
        for /d %%D in ("%%L:\Epic Games\UE_*") do if exist "%%~fD\Engine\Build\BatchFiles\Build.bat" set "ENGINE=%%~fD"
        for /d %%D in ("%%L:\Program Files\Epic Games\UE_*") do if exist "%%~fD\Engine\Build\BatchFiles\Build.bat" set "ENGINE=%%~fD"
        for /d %%D in ("%%L:\Games\Epic Games\UE_*") do if exist "%%~fD\Engine\Build\BatchFiles\Build.bat" set "ENGINE=%%~fD"
        for /d %%D in ("%%L:\Games\UE_*") do if exist "%%~fD\Engine\Build\BatchFiles\Build.bat" set "ENGINE=%%~fD"
        for /d %%D in ("%%L:\UE_*") do if exist "%%~fD\Engine\Build\BatchFiles\Build.bat" set "ENGINE=%%~fD"
        for /d %%D in ("%%L:\Unreal Engine\UE_*") do if exist "%%~fD\Engine\Build\BatchFiles\Build.bat" set "ENGINE=%%~fD"
    )
)

rem 4. Last resort: ask.
if not defined ENGINE (
    echo I could not find Unreal Engine automatically, but you can tell me
    echo where it is - this only has to be done once.
    echo.
    echo The folder is usually called UE_5.8 and contains an Engine folder.
    echo Tip: in the Epic Games Launcher, the engine tile's dropdown arrow
    echo has an option to open its install folder.
    echo.
    set /p ENGINE=Paste the engine folder path here and press Enter:
    set ENGINE=!ENGINE:"=!
)

if not exist "!ENGINE!\Engine\Build\BatchFiles\Build.bat" (
    echo.
    echo That does not look like an Unreal Engine folder. I checked for:
    echo   !ENGINE!\Engine\Build\BatchFiles\Build.bat
    echo Run this script again and double-check the path.
    pause
    exit /b 1
)

rem Remember it for next time.
>"%~dp0EnginePath.txt" echo !ENGINE!
echo Found Unreal Engine:  !ENGINE!

rem ---- Check Visual Studio C++ tools --------------------------------------
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSPATH="
if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%V in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSPATH=%%V"
)
if not defined VSPATH (
    echo Visual Studio 2022 with C++ tools was NOT found - the game cannot
    echo compile without it.
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
