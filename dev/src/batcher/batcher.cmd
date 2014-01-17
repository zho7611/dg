@echo off

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
set res_path=..\..\res\
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

:scooter
if not exist %res_path%scooter.dae goto astro
echo [batcher] build scooter res...
%importer_path% dev\res\scooter.dae res\scooter.pak -hidewindow
if not errorlevel == 0 goto label_error

:astro
if not exist %res_path%astro.dae goto fireman
echo [batcher] build astro res...
%importer_path% dev\res\astro.dae res\astro.pak -hidewindow
if not errorlevel == 0 goto label_error

:fireman
if not exist %res_path%fireman.dae goto huriza
echo [batcher] build fireman res...
%importer_path% dev\res\fireman.dae res\fireman.pak -hidewindow
if not errorlevel == 0 goto label_error

:huriza
if not exist %res_path%huriza.dae goto cammy
echo [batcher] build huriza res...
%importer_path% dev\res\huriza.dae res\huriza.pak -hidewindow
if not errorlevel == 0 goto label_error

:cammy
if not exist %res_path%cammy.dae goto zhom
echo [batcher] build cammy res...
%importer_path% dev\res\cammy.dae res\cammy.pak -hidewindow
if not errorlevel == 0 goto label_error

:zhom
if not exist %res_path%zhom.dae goto house
echo [batcher] build zhom res...
%importer_path% dev\res\zhom.dae res\zhom.pak -hidewindow
if not errorlevel == 0 goto label_error

:house
if not exist %res_path%house.dae goto street
echo [batcher] build house res...
%importer_path% dev\res\house.dae res\house.pak -hidewindow
if not errorlevel == 0 goto label_error

:street
if not exist %res_path%street.dae goto label_end
echo [batcher] build street res...
%importer_path% dev\res\street.dae res\street.pak -hidewindow
if not errorlevel == 0 goto label_error

goto label_end

:label_error
echo [batcher] failed
rem exit 1

:label_end
echo [batcher] finished