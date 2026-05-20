@echo off
REM Build MikasBibleApp.chm from the HTML Help source files.
REM Requires HTML Help Workshop (hhc.exe) to be installed.
REM Default install path: C:\Program Files (x86)\HTML Help Workshop\hhc.exe

set HHC="C:\Program Files (x86)\HTML Help Workshop\hhc.exe"

if not exist %HHC% (
    echo ERROR: HTML Help Workshop not found at %HHC%
    echo Download it from: https://learn.microsoft.com/en-us/previous-versions/windows/desktop/htmlhelp/microsoft-html-help-downloads
    exit /b 1
)

echo Building MikasBibleApp.chm ...
%HHC% MikasBibleApp.hhp

if exist MikasBibleApp.chm (
    echo.
    echo SUCCESS: MikasBibleApp.chm created.
    echo Copy it next to MikasBibleApp.exe before running the application.
) else (
    echo.
    echo WARNING: hhc.exe returned a non-zero exit code.
    echo Check output above for errors.
)
