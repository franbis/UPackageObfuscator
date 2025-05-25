#pragma once

#include <sstream>



#define NAME_TO_TCHAR(name) FName::GetEntry(name.GetIndex())->Name


inline std::string intToStr(int num) {
    std::stringstream ss;
    ss << num;
    return ss.str();
}