@echo off
SET PATH=C:\!Work\link;%PATH%
SET LNKDIR=c:\!work\link
C:\!Work\link\nmake /f winmake.mak /S %1
