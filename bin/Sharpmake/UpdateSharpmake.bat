@echo off

set curdir=%~dp0
set smroot=%~dp0\..\..\..\Sharpmake

set smproj=Sharpmake.Application\Sharpmake.Application_core.csproj
set smsln=Sharpmake.sln

echo Compiling for all platforms
call %smroot%\bootstrap-sharpmake.bat
if %errorlevel% NEQ 0 goto error
call %smroot%\CompileSharpmake.bat %smsln% Release "Any CPU"
if %errorlevel% NEQ 0 goto error
call %smroot%\CompileSharpmake_core.bat %smproj% Release osx-x64
if %errorlevel% NEQ 0 goto error
call %smroot%\CompileSharpmake_core.bat %smproj% Release linux-x64
if %errorlevel% NEQ 0 goto error

set smbin=%smroot%\bin\release\netcoreapp3.0
set smwin=%smroot%\bin\release
set smosx=%smbin%\osx-x64\publish
set smlinux=%smbin%\linux-x64\publish

echo Copying files

mkdir %curdir%\win-x64
mkdir %curdir%\osx-x64
mkdir %curdir%\linux-x64

copy %smwin%\*.exe %curdir%\win-x64 /Y
copy %smwin%\*.dll %curdir%\win-x64 /Y
copy %smwin%\*.pdb %curdir%\win-x64 /Y
copy %smwin%\*.xml %curdir%\win-x64 /Y
if %errorlevel% NEQ 0 goto error
copy %smosx%\*.* %curdir%\osx-x64 /Y
if %errorlevel% NEQ 0 goto error
copy %smlinux%\*.* %curdir%\linux-x64 /Y
if %errorlevel% NEQ 0 goto error

goto success

@REM -----------------------------------------------------------------------
:success
COLOR 2F
echo Update succeeded^!
timeout /t 5
set ERROR_CODE=0
goto end

@REM -----------------------------------------------------------------------
:error
COLOR 4F
echo Update failed^!
pause
set ERROR_CODE=1
goto end

@REM -----------------------------------------------------------------------
:end
:: restore caller current directory
cd %curdir%
exit /b %ERROR_CODE%
