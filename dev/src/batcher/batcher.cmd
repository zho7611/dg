rem @echo off

rem ----------------------------------------
rem batcher commands
rem ----------------------------------------

echo [batcher] running batcher.cmd...
chdir

rem ----------------------------------------
rem clear system files
rem ----------------------------------------

if exist ..\..\..\res\generated\*.bin del ..\..\..\res\generated\*.bin
if not %errorlevel% == 0 goto label_error
if exist ..\..\..\res\generated\*.pak del ..\..\..\res\generated\*.pak
if not %errorlevel% == 0 goto label_error
if exist ..\..\..\res\engine\*.bin del ..\..\..\res\engine\*.bin
if not %errorlevel% == 0 goto label_error
if exist ..\..\..\res\engine\*.pak del ..\..\..\res\engine\*.pak
if not %errorlevel% == 0 goto label_error

rem ----------------------------------------
rem set the debug prefix
rem ----------------------------------------

set exe_configuration=%1
echo [batcher] build configuration = %1

if not %errorlevel% == 0 goto label_error

rem ----------------------------------------
rem run the batch jobs
rem ----------------------------------------

set batcher_path=..\..\..\bin\batcher%exe_configuration%.exe
set importer_path=..\..\..\bin\collada_importer%exe_configuration%.exe

echo [batcher] generate default res...
%batcher_path% -generate -silentexit -hidewindow
if not %errorlevel% == 0 goto label_error

echo [batcher] compile shaders...
%batcher_path% -compileshader -silentexit -hidewindow
if not %errorlevel% == 0 goto label_error

echo [batcher] packer test...
%batcher_path% -packer -t -silentexit -hidewindow
if not %errorlevel% == 0 goto label_error

echo [batcher] deleting packages...
if exist ..\..\..\res\*.pak del ..\..\..\res\*.pak
if not errorlevel == 0 goto label_error

set res_path=..\..\res\

:scooter
set res_name=scooter
if not exist %res_path%%res_name%\%res_name%.dae goto astro
echo [batcher] build %res_name% resource...
%importer_path% dev\res\%res_name%\%res_name%.dae res\%res_name%.pak -hidewindow
if not errorlevel == 0 goto label_error

:astro
set res_name=astro
if not exist %res_path%%res_name%\%res_name%.dae goto fireman
echo [batcher] build %res_name% resource...
%importer_path% dev\res\%res_name%\%res_name%.dae res\%res_name%.pak -hidewindow
if not errorlevel == 0 goto label_error

:fireman
set res_name=fireman
if not exist %res_path%%res_name%\%res_name%.dae goto huriza
echo [batcher] build %res_name% resource...
%importer_path% dev\res\%res_name%\%res_name%.dae res\%res_name%.pak -hidewindow
if not errorlevel == 0 goto label_error

:huriza
set res_name=huriza
if not exist %res_path%%res_name%\%res_name%.dae goto cammy
echo [batcher] build %res_name% resource...
%importer_path% dev\res\%res_name%\%res_name%.dae res\%res_name%.pak -hidewindow
if not errorlevel == 0 goto label_error

:cammy
set res_name=cammy
if not exist %res_path%%res_name%\%res_name%.dae goto zhom
echo [batcher] build %res_name% resource...
%importer_path% dev\res\%res_name%\%res_name%.dae res\%res_name%.pak -hidewindow
if not errorlevel == 0 goto label_error

:zhom
set res_name=zhom
if not exist %res_path%%res_name%\%res_name%.dae goto house
echo [batcher] build %res_name% resource...
%importer_path% dev\res\%res_name%\%res_name%.dae res\%res_name%.pak -hidewindow
if not errorlevel == 0 goto label_error

:house
set res_name=house
if not exist %res_path%%res_name%\%res_name%.dae goto street
echo [batcher] build %res_name% resource...
%importer_path% dev\res\%res_name%\%res_name%.dae res\%res_name%.pak -hidewindow
if not errorlevel == 0 goto label_error

:street
set res_name=street
if not exist %res_path%%res_name%\%res_name%.dae goto label_end
echo [batcher] build %res_name% resource...
%importer_path% dev\res\%res_name%\%res_name%.dae res\%res_name%.pak -hidewindow
if not errorlevel == 0 goto label_error

goto label_end

:label_error
echo [batcher] failed
rem exit 1

:label_end
echo [batcher] finished