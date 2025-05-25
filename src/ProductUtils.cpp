#pragma once

// Link the Version Information APIs.
#pragma comment(lib, "Version.lib")

#include <windows.h>
#include <vector>

#include "ProductUtils.h"



/*
    Simplify a version string by removing trailing zeros.
*/
void getShortVersion(wchar_t** s) {
    size_t len = std::wcslen(*s);
    while (len > 1 && (*s)[len - 2] == L'.' && (*s)[len - 1] == L'0') {
        (*s)[len - 2] = L'\0';
        len -= 2;
    }
}


/*
    Extract the product's name and version from the version info block using
    the Version Information APIs.
*/
bool getProductInfo(wchar_t** productName, wchar_t** productVersion) {
    wchar_t szFilename[MAX_PATH] = { 0 };
    if (GetModuleFileNameW(NULL, szFilename, MAX_PATH) == 0)
        return false;

    // Get the version info block size within the executable.
    DWORD dummy = 0;
    DWORD dwSize = GetFileVersionInfoSizeW(szFilename, &dummy);
    if (dwSize == 0)
        // No version info block present.
        return false;

    // Load the version info into memory.
    std::vector<BYTE> data(dwSize);
    if (!GetFileVersionInfoW(szFilename, 0, dwSize, data.data()))
        return false;

    // Get the translation info, needed to build the subblock paths according to
    // the localization.

    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    } *lpTranslate = nullptr;

    UINT cbTranslate = 0;
    if (!VerQueryValueW(data.data(), L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &cbTranslate) || cbTranslate < sizeof(LANGANDCODEPAGE))
        return false;

    // Build the subblock paths.

    wchar_t subBlockProductName[50] = { 0 };
    wchar_t subBlockProductVersion[50] = { 0 };

    swprintf(subBlockProductName, 50, L"\\StringFileInfo\\%04x%04x\\ProductName", lpTranslate->wLanguage, lpTranslate->wCodePage);
    swprintf(subBlockProductVersion, 50, L"\\StringFileInfo\\%04x%04x\\ProductVersion", lpTranslate->wLanguage, lpTranslate->wCodePage);

    // Extract the field values.

    LPVOID pvProductName = nullptr;
    UINT lenProductName = 0;
    if (!VerQueryValueW(data.data(), subBlockProductName, &pvProductName, &lenProductName) || lenProductName == 0)
        return false;

    LPVOID pvProductVersion = nullptr;
    UINT lenProductVersion = 0;
    if (!VerQueryValueW(data.data(), subBlockProductVersion, &pvProductVersion, &lenProductVersion) || lenProductVersion == 0)
        return false;

    // Copy the values to output.

    *productName = new wchar_t[lenProductName];
    memcpy(*productName, pvProductName, lenProductName * sizeof(wchar_t));

    *productVersion = new wchar_t[lenProductVersion];
    memcpy(*productVersion, pvProductVersion, lenProductVersion * sizeof(wchar_t));

    return true;
}