std::string mkBogusStr(int length);
std::string mkBogusStr(const TCHAR* name);
void exportNames(ULinkerLoad* LL, const TCHAR* filepath, int maxBuiltinNamesSlot, bool printIgnoredNames);
void loadNames(const TCHAR* iniPath, TArray<FString>& names);
bool shouldBogus(const TCHAR* name, TArray<FString>* names, const TCHAR* prefix, const TCHAR* suffix);
void obfNames(TArray<FName>* nameMap, TArray<FString>* iniNames, const TCHAR* prefix, const TCHAR* suffix, bool useNameHash, int nameLength);

void modifyScriptText(UTextBuffer*& scriptText, const TCHAR* filepath, bool null, bool del, bool verbose);
void modifyAllScriptTexts(UObject* pkg, const TCHAR* filepath, bool null, bool del, bool verbose);

void addObfInfo(UObject* obj, TCHAR* productName, TCHAR* productVersion);
UTextBuffer* getObfInfo(UObject* obj);