#pragma once

#include "Main.h"



void printSoftwareInfo(TCHAR* productName, TCHAR* productVersion) {
	PRINTF("")
	PRINTF_INFO("Unreal Engine v%i%s", ENGINE_VERSION, ENGINE_REVISION)
	PRINTF_INFO("%s v%s", productName, productVersion)
	PRINTF("")
}


void printHelp(const TCHAR* progName) {
	PRINTF_INFO("Usage:")
	PRINTF_INFO("	%s <help|input_package_path> [options]", progName)
	PRINTF_INFO("Options:")
	PRINTF_INFO("	exportnames=<filepath>		Export the names found in the input package to an ini file and exit.")
	PRINTF_INFO("	names=<filepath>		Read which names to obfuscate from an ini file.")
	PRINTF_INFO("	prefix=<prefix>			Any name with this prefix will be obfuscated. Ignored if 'names' is set.")
	PRINTF_INFO("	suffix=<suffix>			Any name with this suffix will be obfuscated. Ignored if 'names' is set.")
	PRINTF_INFO("	usehash=<0|1>			Base the bogus strings on the name hashes rather than generating a random one.")
	PRINTF_INFO("	namelen=<length>		The length each bogus name must have. (Default: 20)")
	PRINTF_INFO("	scr=<filepath>			Replace the text in the ScriptText objects with the content of a file.")
	PRINTF_INFO("	delscr=<0|1>			Delete the ScriptText object in an attempt to crash deobfuscators when loading the package")
	PRINTF_INFO("	noprompt=<0|1>			Do not prompt anything, always use the default behavior.")
	PRINTF_INFO("	noobfinfo=<0|1>			Do not add the obfuscation info object to the package.")
	PRINTF_INFO("	out=<filepath>			The output package path. If not provided, the input package path will be used.")
	PRINTF_INFO("	conform=<0|1>			Conform the output package.")
	PRINTF_INFO("	checkobf=<0|1>			Print the content of the obfuscation info object if present in the input package and exit.")
}


/*
	Parse the command line parameters.
*/
void parseParams(
	const char** argv,
	FString& inPkgPath, FString& namesIniPathOut, FString& namesIniPathIn, FString& prefix,
	FString& suffix, UBOOL& useNameHash, INT& nameLength, FString& scriptTextPath,
	UBOOL& shouldDelScr, UBOOL& noPrompt, FString& outPkgPath, UBOOL& shouldConform,
	UBOOL& noObfInfo, UBOOL& checkObf
)
{
	// Set defaults.
	inPkgPath = namesIniPathOut = namesIniPathIn = prefix = suffix = scriptTextPath
	= outPkgPath = FString("");
	nameLength = 20;
	useNameHash = shouldDelScr = noPrompt = shouldConform = noObfInfo = checkObf = 0;

	inPkgPath = FString(argv[1]);


	GET_PARAM_W_KEY_TO_REF(namesIniPathOut, "exportnames")

	GET_PARAM_W_KEY_TO_REF(namesIniPathIn, "names")
	GET_PARAM_TO_REF(prefix)
	GET_PARAM_TO_REF(suffix)
	GET_PARAM_W_KEY_TO_REF(useNameHash, "usehash")
	GET_PARAM_W_KEY_TO_REF(nameLength, "namelen")

	GET_PARAM_W_KEY_TO_REF(scriptTextPath, "scr")
	GET_PARAM_W_KEY_TO_REF(shouldDelScr, "delscr")
	
	GET_PARAM_W_KEY_TO_REF(noPrompt, "noprompt")
	GET_PARAM_W_KEY_TO_REF(outPkgPath, "out")
	GET_PARAM_W_KEY_TO_REF(shouldConform, "conform");

	GET_PARAM_W_KEY_TO_REF(noObfInfo, "noobfinfo");

	GET_PARAM_W_KEY_TO_REF(checkObf, "checkobf");
}


/*
	Return the LinkerLoad for a package.
*/
ULinkerLoad* getLL(const TCHAR* filepath, DWORD loadFlags = LOAD_NoFail) {
	UObject::BeginLoad();
	ULinkerLoad* LL = UObject::GetPackageLinker(NULL, filepath, loadFlags, NULL, NULL);
	UObject::EndLoad();
	return LL;
}


/*
	Return a package given its path.
*/
UPackage* getPkg(const TCHAR* filepath, DWORD loadFlags = LOAD_NoFail) {
	UPackage* pkg = Cast<UPackage>(UObject::LoadPackage(NULL, filepath, loadFlags));
	return pkg;
}


/*
	Print the content of the obfuscation info object within the given
	object, if present.
*/
void printObfInfo(UObject* obj) {
	UTextBuffer* obfInfo = getObfInfo(obj);
	if (obfInfo) {
		PRINTF_INFO("The obfuscation info object was found in the package.")
		PRINTF_INFO("Content:")
		PRINTF("")
		PRINTF_INFO("%s", obfInfo->Text)
		PRINTF("")
	}
	else {
		PRINTF_INFO("The obfuscation info object was not found in the package.")
	}
}

/*
	Save an object as package to the specified path.
*/
void savePkg(UObject* pkgObj, const TCHAR* filepath, ULinkerLoad* conformLL = NULL) {
	// NOTE: SavePackage() crashes when conforming if running through VisualStudio.
	UObject::SavePackage(pkgObj, NULL, RF_Standalone, filepath, GError, conformLL);
}


// Use "const char** argv" so that we can easily convert argvs to FString using
// catCmdLine().
int main(int argc, const char** argv) {
	INT ErrorLevel = 0;
	GIsStarted = 1;

	try {
		GIsGuarded = 1;


		TCHAR* productName;
		TCHAR* productVersion;
		getProductInfo(&productName, &productVersion);
		// + 1 to account for the null terminator.
		TCHAR* shortProductVersion = new TCHAR[appStrlen(productVersion) + 1];
		appStrcpy(shortProductVersion, productVersion);
		getShortVersion(&shortProductVersion);


		// Pass 'Warn' instead of 'Log' to print directly onto the cmd rather
		// than to a log file.
		// We don't use the official way of concatenating the cmdline as done
		// in UE because the latter likely writes to argv.
		appInit(
			productName,
			*catCmdLine(argc, argv),
			&Malloc, &Warn, &Error, &Warn, &FileManager,
			FConfigCacheIni::Factory,
			0
		);
		// Standalone.
		GIsClient = GIsServer = GIsEditor = GIsScriptable = 1;


		// Highest built-in name index, used to handle built-in names differently.
		int maxBuiltinNamesSlot = FName::GetMaxNames();


		printSoftwareInfo(productName, shortProductVersion);


		// Print the help if needed.
		if ((argc == 1) || (appStrcmp(ANSI_TO_TCHAR(argv[1]), TEXT("help")) == 0)) {
			printHelp(ANSI_TO_TCHAR( GET_CLI_NAME(argv[0]).c_str() ));
			goto finish;
		}


		// Parameters parsing.
		FString inPkgPath, namesIniPathOut, namesIniPathIn, prefix, suffix, scriptTextPath,
			outPkgPath;
		INT nameLength;
		UBOOL useNameHash, shouldDelScr, noPrompt, shouldConform, noObfInfo, checkObf;
		parseParams(
			argv,
			inPkgPath, namesIniPathOut, namesIniPathIn, prefix, suffix, useNameHash,
			nameLength, scriptTextPath, shouldDelScr, noPrompt, outPkgPath, shouldConform,
			noObfInfo, checkObf
		);


		// Load the pkg and its linker without verifying the deps so we can
		// run ops that don't require to load any dep.
		// We don't need to call LoadAllObjects() on the linker later on as
		// deps are loaded on demand anyway.
		// 
		// NOTE: Saving the pkg still requires the deps. This means the ops
		//			that don't rely on the deps but lead to the saving of a
		//			pkg will still, indirectly, require the deps.

		ULinkerLoad* inLinkerLoad = getLL(*inPkgPath, LOAD_NoVerify);
		UPackage* inPkg = getPkg(*inPkgPath, LOAD_NoVerify);


		// Check if the package was loaded successfully, abort if it wasn't.
		if (!inPkg) {
			ErrorLevel = 1;
			PRINTF_ERR("Error: The input package could not be loaded.")
			goto finish;
		}


		// Check if the specified script text file exists, abort if it doesn't.
		if (appStrlen(*scriptTextPath) && !fileExists(*scriptTextPath)) {
			ErrorLevel = 1;
			PRINTF_ERR("The script text file at '%s' could not be found.", *scriptTextPath)
				goto finish;
		}


		PRINTF_INFO("Internal package name: %s", inPkg->GetFullName())


		/////////////////////////
		// Standalone ops.


		// Save names to file.
		if (namesIniPathOut.Len()) {
			PRINTF_INFO("Saving names...")
			exportNames(inLinkerLoad, *namesIniPathOut, maxBuiltinNamesSlot, true);
			goto finish;
		}
		

		// Check if input pkg is obfuscated.
		if (checkObf) {
			printObfInfo(inPkg);
			goto finish;
		}


		/////////////////////////
		// Checks & Prompts.


		// Check if the output package was provided.
		if (!outPkgPath.Len()) {
			UBOOL goodToGo = 1;
			PRINTF_WARN("The output package was not provided, the input package will be overwritten.")
			if (!noPrompt) {
				COLOR_BEGIN(CMDC_WARN)
					goodToGo = GWarn->YesNof(TEXT("Do you wish to continue?"));
				COLOR_END()
			}
			if (!goodToGo)
				goto finish;

			outPkgPath = FString(inPkgPath);
		}
		else {
			// Stop if the outPkgPath is being used by another process.
			if (isFileBeingUsed(*outPkgPath)) {
				UBOOL goodToGo = 1;
				PRINTF_WARN("A new package cannot be created at '%s' because one already exists at that path and could not be deleted.", &outPkgPath)
				PRINTF_WARN("This may be due to the file being opened by another process.")
				if (!noPrompt) {
					COLOR_BEGIN(CMDC_WARN)
						goodToGo = GWarn->YesNof(TEXT("Do you wish to continue? (You will need to manually close any process that is using that file before continuing)"));
					COLOR_END()
				}
				if (!goodToGo)
					goto finish;
			}
		}


		// Check if the Engine's version is compatible with the UEExplorer bug.
		if ((prefix.Len() + suffix.Len() + namesIniPathIn.Len()) <= 0) {
			UBOOL goodToGo = 1;
			PRINTF_WARN("Neither a names list file, a prefix or a suffix was specified, names won't be obfuscated.")
			PRINTF_WARN("ScriptText object may still be modified.")
			if (!noPrompt) {
				COLOR_BEGIN(CMDC_WARN)
					goodToGo = GWarn->YesNof(TEXT("Do you wish to continue?"));
				COLOR_END()
			}
			if (!goodToGo)
				goto finish;
		}


		// Check if the Engine's version is compatible with the UEExplorer bug.
		if (shouldDelScr && (ENGINE_VERSION == 469) && appStrcmp(ENGINE_REVISION, TEXT("d"))) {
			PRINTF_WARN("Deleting ScriptText objects was only tested on 469d, it may not work on this version (%i%s).", ENGINE_VERSION, ENGINE_REVISION)
			if (!noPrompt) {
				COLOR_BEGIN(CMDC_WARN)
					shouldDelScr = GWarn->YesNof(TEXT("Do you wish to continue?"));
				COLOR_END()
			}
		}


		// Get the names from the ini.
		TArray<FString> iniNames;
		if (namesIniPathIn.Len() != 0) {
			loadNames(*namesIniPathIn, iniNames);
			if (!iniNames.Num()) {
				PRINTF_WARN("No names found in the ini file '%s'.", *namesIniPathIn)
			}
		}


		/////////////////////////
		// Ops that lead to the saving of the output package.


		// Names obfuscation.
		TArray<FName> nameMap = inLinkerLoad->NameMap;
		obfNames(
			&nameMap, &iniNames,
			*prefix, *suffix,
			useNameHash, nameLength
		);


		// From here on, all the ops require the package depencencies to
		// be loaded.


		// ScriptText handling.
		modifyAllScriptTexts(
			inPkg,
			*scriptTextPath,
			!scriptTextPath.Len() && !shouldDelScr,
			shouldDelScr,
			true
		);


		// Add the obfuscation info object.
		if (!noObfInfo)
			addObfInfo(inPkg, productName, shortProductVersion);


		// Save the pkg and conform.
		ULinkerLoad* oldLL = shouldConform ? inLinkerLoad : NULL;
		PRINTF_INFO("Saving the package...")
		savePkg(Cast<UObject>(inPkg), *outPkgPath, oldLL);
		PRINTF_INFO("Output package saved to '%s'.", outPkgPath)

	}
	catch (...) {
		ErrorLevel = 1;
		GIsGuarded = 0;
		Error.HandleError();
	}

finish:

	appExit();
	GIsStarted = 0;
	return ErrorLevel;
}