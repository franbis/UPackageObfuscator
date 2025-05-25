#pragma once

#include "Core.h"
#include "FileUtils.h"


#define GET_CLI_NAME(filepath) STRIP_EXT(GET_BASE_NAME(std::string(filepath)))

#define GET_PARAM(type, defaul, param) type param = defaul; Parse(appCmdLine(), L#param TEXT("="), param);
#define GET_PARAM_TO_REF(param) Parse(appCmdLine(), L#param TEXT("="), param);
#define GET_PARAM_W_KEY(type, defaul, param, key) type param = defaul; Parse(appCmdLine(), TEXT(key) TEXT("="), param);
#define GET_PARAM_W_KEY_TO_REF(param, key) Parse(appCmdLine(), TEXT(key) TEXT("="), param);

#define PRINTF(msg, ...) GLog->Logf(TEXT(msg), ##__VA_ARGS__);


/*
	Return the command line arguments concatenated into an FString.
*/
inline FString catCmdLine(int argc, const char** argv) {
	FString full;

	for (int i = 1; i < argc; i++) {
		full += FString(" ");
		full += FString(argv[i]);
	}

	return full;
}