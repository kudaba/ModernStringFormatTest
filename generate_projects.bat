call bin\Sharpmake\win-x64\Sharpmake.Application.exe "/sources(@'SharpMake\Main.cs') /verbose /generateDebugSolution %*"
if "%errorlevel%" == "0" goto end

@COLOR 4F
pause
exit 1

:end
