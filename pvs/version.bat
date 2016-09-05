@echo off

for %%I in (%1) do set RevMaker=%%~sI
for /f "delims== tokens=2 usebackq" %%I in (%2) do set SVNRevBin=%%~sI
for %%I in (%3) do set TargetPath=%%~sI
REM 为了不修改原文件名，不能再改编该串了
REM for %%I in (%4) do set TargetFile=%%~sI
set TargetFile=%4
set PrevVer=1.3.0
echo %PrevVer%

%RevMaker%  %SVNRevBin%  %TargetPath%  %TargetFile%  %PrevVer%
