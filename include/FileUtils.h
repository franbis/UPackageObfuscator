#define GET_BASE_NAME(path) path.substr(path.find_last_of("/\\") + 1)
#define STRIP_EXT(basename) basename.substr(0, basename.find_last_of('.'))

#define FILE_WRITE(arc, s) arc->Serialize(const_cast<TCHAR*>(s), appStrlen(s) * sizeof(TCHAR));


bool fileExists(const TCHAR* filepath);
bool isFileBeingUsed(const TCHAR* filepath);