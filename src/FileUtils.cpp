#pragma once

#include <Core.h>



bool fileExists(const TCHAR* filepath) {
	return GFileManager->FileSize(filepath) != -1;
}


/*
	Return true if a file exists and is being used by another process. Otherwise
	return false.
*/
bool isFileBeingUsed(const TCHAR* filepath) {
	try {
		/*
			Try to create a file with write mode, if we can't (FILEWRITE_NoFail) then
			there's already one at that path and it is being used by another process.
			Use FILEWRITE_Append to avoid replacing an existing file with a new one,
			in case we are able to open it.
		*/
		FArchive* arc = GFileManager->CreateFileWriter(filepath, FILEWRITE_NoFail | FILEWRITE_Append);
		delete arc;
		if (fileExists(filepath))
			// The file didn't exist, an empty one was created, so delete it for
			// completeness.
			GFileManager->Delete(filepath);

		return false;
	}
	catch (...) {
		return true;
	}
}