@echo off


set OBF_SUFFIX=-obfuscated
set BIN_PATH=..\bin
set EXAMPLES_PATH=..\examples


goto main


:obfuscate
	UPackageObfuscator %1.u prefix=zz suffix=_ scr=%EXAMPLES_PATH%\example_script_text.txt noprompt=1
	echo %errorlevel%
	if %errorlevel% neq 0 (
		pause
		exit
	)
	exit /B


:main
	echo.

	cd %BIN_PATH%

	if not exist "Core.u" goto :abort
	if not exist "Engine.u" goto abort

	del %EXAMPLES_PATH%\*%OBF_SUFFIX%.u
	copy %EXAMPLES_PATH%\ExamplePackage*.u .
	call :obfuscate ExamplePackage1
	call :obfuscate ExamplePackage2
	for %%f in (ExamplePackage*.u) do move "%%f" "%EXAMPLES_PATH%\%%~nf-obfuscated.u"

	echo.
	pause
	exit


:abort
	echo Error: Both Core.u and Engine.u must be present in the application directory to run this example.

	echo.
	pause
	exit