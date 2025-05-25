#pragma once

#include "CommandLine.h"



#define CMDC_BLACK			0
#define CMDC_BLUE			1
#define CMDC_GREEN			2
#define CMDC_AQUA			3
#define CMDC_RED			4
#define CMDC_PURPLE			5
#define CMDC_YELLOW			6
#define CMDC_WHITE			7
#define CMDC_GRAY			8
#define CMDC_LIGHT_BLUE		9
#define CMDC_LIGHT_GREEN	10
#define CMDC_LIGHT_AQUA		11
#define CMDC_LIGHT_RED		12
#define CMDC_LIGHT_PURPLE	13
#define CMDC_LIGHT_YELLOW	14
#define CMDC_BRIGHT_WHITE	15

#define CMDC_DEFAULT		CMDC_BRIGHT_WHITE
#define CMDC_INFO			CMDC_LIGHT_GREEN
#define CMDC_WARN			CMDC_LIGHT_YELLOW
#define CMDC_ERR			CMDC_LIGHT_RED


#ifdef WIN32
	inline HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	#define COLOR_BEGIN(color) SetConsoleTextAttribute(hConsole, color);
	#define COLOR_END() SetConsoleTextAttribute(hConsole, CMDC_DEFAULT);

	#define PRINTF_COLORED(color, msg, ...) COLOR_BEGIN(color) PRINTF(msg, ##__VA_ARGS__) COLOR_END()
	#define PRINTF_INFO(msg, ...) PRINTF_COLORED(CMDC_INFO, msg, ##__VA_ARGS__)
	#define PRINTF_WARN(msg, ...) PRINTF_COLORED(CMDC_WARN, msg, ##__VA_ARGS__)
	#define PRINTF_ERR(msg, ...) PRINTF_COLORED(CMDC_ERR, msg, ##__VA_ARGS__)
#else
	// TODO - Needs testing.
	#define COLOR_BEGIN(color)
	#define COLOR_END()

	#define PRINTF_COLORED(color, msg, ...) PRINTF(msg, ##__VA_ARGS__)
	#define PRINTF_INFO(msg, ...) PRINTF(msg, ##__VA_ARGS__)
	#define PRINTF_WARN(msg, ...) PRINTF(msg, ##__VA_ARGS__)
	#define PRINTF_ERR(msg, ...) PRINTF(msg, ##__VA_ARGS__)
#endif


#ifdef DEBUG
	//Technically called: process control patches.
	#define PCP_TEST "PCP Test"
	print(PCP_TEST);
	printColored(CMDC_AQUA, PCP_TEST);
	printInfo(PCP_TEST);
	printWarn(PCP_TEST);
	printErr(PCP_TEST);
#endif