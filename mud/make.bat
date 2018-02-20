@echo off
SET LNKDIR=c:\!work\link
SET PATH=%LNKDIR%;%PATH%
nmake.exe /f winmake.mak /S %1
