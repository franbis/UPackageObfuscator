@echo off


goto main


:setTmpPath
	@echo off

	set tmpPath=

	:: Prompt
	set /P tmpPath=Input the %* path (or leave empty to use the picker):

	:: If empty, use folder picker via mshta
	if not "%tmpPath%"=="" (exit /B)

	:: Create a temporary VBScript file to open the folder picker
	set "vbsFile=%temp%\SelectFolder.vbs"
	echo Set objDialog = CreateObject("Shell.Application") > "%vbsFile%"
	echo Set objFolder = objDialog.BrowseForFolder(0, "Select a folder:", 0, 0) >> "%vbsFile%"
	echo If Not objFolder Is Nothing Then >> "%vbsFile%"
	echo     Wscript.Echo objFolder.Self.Path >> "%vbsFile%"
	echo End If >> "%vbsFile%"

	:: Run the VBScript and capture the output
	for /f "usebackq delims=" %%I in (`cscript //nologo "%vbsFile%"`) do set "tmpPath=%%I"

	:: Clean up
	del "%vbsFile%"

	exit /B


:copyFileFromTmpPath
	copy %tmpPath%\%1 . >nul 2>&1
	if errorlevel 1 (
		echo '%1' could not be found at path '%tmpPath%'.
		call :finish 1
	)
	
	exit /B


:main
	echo.

	call :setTmpPath Unreal Engine DLLs
	call :copyFileFromTmpPath Core.dll
	call :copyFileFromTmpPath Engine.dll
	
	call :setTmpPath Unreal Engine Localization (.int files)
	call :copyFileFromTmpPath Core.int
	call :copyFileFromTmpPath Engine.int
	
	call :finish 0
	

:finish
	echo.
	if %1==0 (
		echo The setup was successful.
	) else (
		echo The setup failed.
	)

	echo.
	pause
	exit