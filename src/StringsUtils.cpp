#pragma once

#include "StringUtils.h"



/*
	Concatenate a set of wchar_t strings and return a pointer to the resulted TCHAR.
*/
TCHAR* catWcharStrings(int count, ...) {
	va_list args;

	// Calc the buffer length based on the length of each string.
	int totalLen = 0;
	va_start(args, count);
	for (int i = 0; i < count; ++i) {
		const TCHAR* str = va_arg(args, const TCHAR*);
		int len = appStrlen(str);
		// appStrlen() doesn't have a documentation, it may return an error code in
		// some cases.
		if (len < 0) len = 0;
		totalLen += len;
		if (totalLen < 0) {
			// The total length overflowed.
			va_end(args);
			return NULL;
		}
	}
	va_end(args);

	// Allocate memory for the buffer.
	TCHAR* buffer = (TCHAR*)malloc((totalLen + 1) * sizeof(TCHAR));
	if (!buffer)
		return NULL;

	// Initialize the buffer with an empty string to let appStrcat() know where to
	// append.
	buffer[0] = TEXT('\0');

	// Concatenate the strings into the buffer.
	va_start(args, count);
	for (int i = 0; i < count; ++i) {
		const TCHAR* str = va_arg(args, const TCHAR*);
		appStrcat(buffer, str);
	}
	va_end(args);

	return buffer;
}