#pragma once

#include <iostream>
#include <sstream>
#include <vector>

#if WIN32
	#include <windows.h>
#else
	// TODO - Needs testing.
	#include <errno.h>
	#include <sys/stat.h>
#endif
#include <malloc.h>
#include <stdio.h>


//#include "Core.h"
// We include CorePrivate.h just to be able to add the ObfInfo obj using
// UTextBufferFactory but, since it contains Core.h and UnLinker.h which we also need,
// it's not a bad idea to include it in general.
#include "CorePrivate.h"
//#include "EditorPrivate.h"
//#include "UnLinker.h"

// Log.
#include "FOutputDeviceFile.h"
FOutputDeviceFile Log;

// Error.
#include "FOutputDeviceAnsiError.h"
FOutputDeviceAnsiError Error;

// Feedback.
#include "FFeedbackContextAnsi.h"
FFeedbackContextAnsi Warn;

// File manager.
#include "FFileManagerWindows.h"
FFileManagerWindows FileManager;
#include "FMallocAnsi.h"
FMallocAnsi Malloc;

// Config.
#include "FConfigCacheIni.h"

// Used for "debugf".
//#include "UnFile.h"


#include "CommandLine.h"
#include "CommandLineColors.h"

#include "Utils.h"
#include "StringUtils.h";
#include "FileUtils.h"
#include "ProductUtils.h";

#include "Obfuscation.h"


//static BOOL GRecoveryMode = FALSE;
//extern const TCHAR* GPackage;


#include "Info.h"