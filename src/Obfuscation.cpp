#pragma once

#include <iostream>
#include <sstream>
#include <set>
#include <random>

#include <CorePrivate.h>
#include <EditorPrivate.h>

#include "StringUtils.h"
#include "CommandLineColors.h"
#include "Utils.h"
#include "FileUtils.h"

#include "Info.h"

#include "Obfuscation.h"



/*
    Generate a random string.
*/
std::string mkBogusStr(int length) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    // Use control characters only to make it as less human-readable as possible.
    static std::uniform_int_distribution<> dist(1, 31);

    std::string result;
    for (int i = 0; i < length; ++i)
        // Interval each char with a newline to make searching by name harder.
        result += (i % 2) ? "\n" : std::string(1, static_cast<char>(dist(gen)));

    return result;
}


/*
    Return a bogus string based on a name's hash.
*/
std::string mkBogusStr(const TCHAR* name) {
    std::string result;

    // Get the name's hash and convert it to string so we can iterate for each
    // symbol.
    std::string hashStr = intToStr(appStrihash(name));
    // Reverse the hashStr to slow down the reverse-engineers.
    std::reverse(hashStr.begin(), hashStr.end());

    // Process each character in the hash string.
    for (char ch : hashStr) {
        if (ch == '-') {
            // We turn the negative sign into chr(11) as the maximum the numbers
            // can be turned into is chr(10).
            result += static_cast<char>(11);
        }
        else if (isdigit(ch)) {
            // When we subtract '0' from any digit character, we get its actual
            // integer value.
            int num = ch - '0';
            // Add 1 to the number so that we don't use chr(0) as it would end
            // the string.
            result += static_cast<char>(num + 1);
        }

        // Interval each char with a newline to make searching by name harder.
        result += '\n';
    }

    return result;
}


/*
    Export a LinkerLoad name map to an ini-formatted file.
    The ini will contain a section and an array, both called 'Names'.

    @param LL                   LinkerLoad that contains the name map.
    @param filepath             Output filepath.
    @param maxBuiltinNamesSlot  Highest built-in name index.
    @param verbose              If true, allow logging. 
*/
void exportNames(ULinkerLoad* LL, const TCHAR* filepath, int maxBuiltinNamesSlot, bool verbose) {
    // Use a set to avoid having duplicates.
    std::set<int> expIdxSet;

    FArchive* arc = GFileManager->CreateFileWriter(filepath, FILEWRITE_EvenIfReadOnly);

    // NOTE: UE can't parse TCHAR ini files if there is no Unicode BOM.
    _WORD bom = UNICODE_BOM;
    arc->Serialize(&bom, 2);

    FILE_WRITE(arc, TEXT("[Names]"))

    TArray<FObjectExport> exportMap = LL->ExportMap;
    for (int i = 0; i < exportMap.Num(); i++) {
        FName objName = exportMap(i).ObjectName;
        int expIdx = objName.GetIndex();

        if (expIdx <= maxBuiltinNamesSlot) {
            if (verbose) {
                PRINTF_WARN("Built-in name '%s' was ignored.", NAME_TO_TCHAR(objName))
            }
            continue;
        }

        if (expIdxSet.find(expIdx) == expIdxSet.end()) {
            FILE_WRITE(arc, TEXT("\r\n"))
            FILE_WRITE(arc, TEXT("Names="))
            FILE_WRITE(arc, NAME_TO_TCHAR(exportMap(i).ObjectName))

            expIdxSet.insert(expIdx);
        }
    }

    arc->Flush();
    delete arc;
}


/*
    Loads an array of name strings from an ini file.
    Both the ini section and array must be called 'Names'.
*/
void loadNames(const TCHAR* iniPath, TArray<FString>& names) {
    TMultiMap<FString, FString>* sec = GConfig->GetSectionPrivate(TEXT("Names"), 0, 0, iniPath);
    if (sec != NULL)
        sec->MultiFind(TEXT("Names"), names);
}


/*
    Return true if a name is either present in a name string array, starts with
    a prefix or ends with a suffix.
*/
bool shouldBogus(
    const TCHAR* name, TArray<FString>* names, const TCHAR* prefix, const TCHAR* suffix
) {
    bool result = false;

    // Check if the name is present in names.
    for (int i = 0; i < names->Num(); i++) {
        result = appStrcmp(name, *(*names)(i)) == 0;
        if (result)
            break;
    }

    // Prefix check.
    if (!result && appStrlen(prefix))
        result = appStrcmp( *FString(name).Left(appStrlen(prefix)), prefix ) == 0;
    
    // Suffix check.
    if (!result && appStrlen(suffix))
        result = appStrcmp(*FString(name).Right(appStrlen(suffix)), suffix) == 0;

    return result;
}


/*
    Obfuscate the names in a LinkerLoad names map according to either a list of names
    or a prefix/suffix.

    @param nameMap          Array of names from a LinkerLoad.
    @param namesToObf       Array of name strings that specifies which name must be
                            obfuscated.
    @param prefix           Prefix a name must have to be obfuscated.
    @param suffix           Suffix a name must have to be obfuscated.
    @param useNameHash      If true, base the bogus strings on the name hashes.
*/
void obfNames(
    TArray<FName>* nameMap, TArray<FString>* namesToObf, const TCHAR* prefix,
    const TCHAR* suffix, bool useNameHash, int nameLength
) {
    // We iterate through all the names as those that are part of external packages are
    // not in the import/export maps apparently. (Pheraps this could be solved by looking for
    // the Dependencies in the package).
    //TArray<FObjectExport> importExportMap = reinterpret_cast<TArray<FObjectExport>&>(inPkgLinkerLoad->ExportMap);
    //importExportMap += reinterpret_cast<TArray<FObjectExport>&>(inPkgLinkerLoad->ExportMap);

    // Note: FName::Rename() was added around 469.

    for (int i = 0; i < nameMap->Num(); i++) {
        FName expName = (*nameMap)(i);
        FNameEntry* nameEntry = FName::GetEntry(expName.GetIndex());

        if (shouldBogus(nameEntry->Name, *&namesToObf, prefix, suffix)) {
            std::string bogusStr;
            if (useNameHash)
                bogusStr = mkBogusStr(nameEntry->Name);
            else
                bogusStr = mkBogusStr(nameLength);
            FString newName(bogusStr.c_str());
            FName::Rename(nameEntry, *newName);
        }
    }
}


/*
    Modify or delete a ScriptText object.

    @param scriptText       ScriptText object.
    @param filepath         If specified, the text within the file will replace the
                            ScriptText object's text.
    @param null             If true the ScriptText object will be set to null.
    @param del              If true the ScriptText object will be deleted.
    @param verbose          If true, allow logging. 
*/
void modifyScriptText(UTextBuffer*& scriptText, const TCHAR* filepath, bool null, bool del, bool verbose) {
    // NOTE: uclassInstance->Script.Empty() was added recently, tho that's the
    //          the official way of erasing a ScriptText object and it probably
    //          leaves some traces.

    #define LOG_TXT "ScriptText object for class '%s' has been %s."

    const TCHAR* clsName = scriptText->GetOuter()->GetName();


    if (null) {
        scriptText = NULL;

        if (verbose) {
            PRINTF_INFO(LOG_TXT, clsName, TEXT("nullified"))
        }
    }

    if (appStrlen(filepath)) {
        FString newText("");
        appLoadFileToString(newText, filepath);
        scriptText->Text = newText;

        if (verbose) {
            PRINTF_INFO(LOG_TXT, clsName, TEXT("replaced"))
        }
    }

    if (del) {
        // Erase first as failsafe.
        scriptText->Text = FString("");
        delete scriptText;

        if (verbose) {
            PRINTF_INFO(LOG_TXT, clsName, TEXT("deleted"))
        }
    }
}


/*
    Modify or delete the ScriptText object for each class in a package.

    @param pkg              The package object.

    (See modifyScriptText() documentation for the other parameters)
*/
void modifyAllScriptTexts(UObject* pkg, const TCHAR* filepath, bool null, bool del, bool verbose) {
    for (TObjectIterator<UClass> It; It; ++It) {
        if (It->GetOuter()->GetName() == pkg->GetName()) {
            if (It->ScriptText)
                modifyScriptText(It->ScriptText, filepath, null, del, verbose);
        }
    }
}


/*
    Create and insert an obfuscation info object into the given object.
    It can also be extracted using:
    'ucc [Editor.]BatchExportCommandlet <package_path> TextBuffer txt <output_dir>'
*/
void addObfInfo(UObject* obj, TCHAR* productName, TCHAR* productVersion) {
    // No need to find out if the obfuscation object already exists in obj, this
    // one will replace it regardless.
    UTextBufferFactory* fac = new UTextBufferFactory();

    TCHAR* text = catWcharStrings(
        7,
        OBF_INFO_TEXT_BEGIN,
        TEXT(" "),
        productName,
        TEXT(" "),
        productVersion,
        TEXT("\n"),
        APP_URL
    );

    fac->FactoryCreateText(
        UTextBuffer::StaticClass(),
        obj,
        OBF_INFO_NAME,
        RF_Public | RF_Standalone,
        NULL,
        // Up to 469e, BatchExportCommandlet ignores this anyway.
        TEXT("txt"),
        (const TCHAR*&)text, text + appStrlen(text),
        GWarn
    );
}


/*
    Return the obfuscation info object if present in the package.
*/
UTextBuffer* getObfInfo(UObject* obj) {
    return FindObject<UTextBuffer>(obj, OBF_INFO_NAME);
}