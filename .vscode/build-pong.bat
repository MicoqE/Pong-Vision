@echo off
setlocal

set "PROJECT_ROOT=%~dp0.."
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    echo ERREUR : vswhere.exe est introuvable : "%VSWHERE%"
    echo Installe Visual Studio Build Tools avec la charge de travail C++ Desktop.
    exit /b 1
)

set "VS_INSTALLATION_PATH="
for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_INSTALLATION_PATH=%%I"

if not defined VS_INSTALLATION_PATH (
    echo ERREUR : aucune installation Visual Studio avec les outils C++ x64 n'a ete trouvee.
    echo Installe Visual Studio Build Tools avec la charge de travail C++ Desktop.
    exit /b 1
)

set "VSDEVCMD=%VS_INSTALLATION_PATH%\Common7\Tools\VsDevCmd.bat"
if not exist "%VSDEVCMD%" (
    echo ERREUR : VsDevCmd.bat est introuvable : "%VSDEVCMD%"
    exit /b 1
)

set "OPENCV_ROOT=%PROJECT_ROOT%\opencv"
set "OPENCV_INCLUDE=%OPENCV_ROOT%\build\include"
set "OPENCV_LIB=%OPENCV_ROOT%\build\x64\vc16\lib\opencv_world500.lib"
set "OPENCV_DLL=%OPENCV_ROOT%\build\x64\vc16\bin\opencv_world500.dll"

if not exist "%OPENCV_INCLUDE%\opencv2\opencv.hpp" goto :opencv_missing
if not exist "%OPENCV_LIB%" goto :opencv_missing
if not exist "%OPENCV_DLL%" goto :opencv_missing

call "%VSDEVCMD%" -arch=amd64 -host_arch=amd64
if errorlevel 1 exit /b %errorlevel%

cl.exe /nologo /EHsc /std:c++17 /Zi /I"%OPENCV_INCLUDE%" "%PROJECT_ROOT%\main.cpp" /link /MACHINE:X64 "%OPENCV_LIB%" /OUT:"%PROJECT_ROOT%\Pong.exe"
exit /b %errorlevel%

:opencv_missing
echo ERREUR : OpenCV 5.0 x64 est introuvable ou incomplet.
echo Telecharge le package Windows depuis https://opencv.org/releases/ et extrais-le dans :
echo   "%OPENCV_ROOT%"
echo Les fichiers requis doivent se trouver sous build\include, build\x64\vc16\lib
echo et build\x64\vc16\bin, avec opencv_world500.lib et opencv_world500.dll.
echo Si ton archive a une autre structure ou un autre nom de bibliotheque, configure-la
echo pour fournir ces chemins et ces noms, ou adapte les variables OPENCV_* dans ce script.
exit /b 1
