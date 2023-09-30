// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"
void DebugPrintA(const char* format, ...) {
    std::string temp;
    va_list marker = { 0 };
    va_start(marker, format);
    size_t num_of_chars = _vscprintf(format, marker);
    if (num_of_chars > temp.capacity()) {
        temp.reserve(num_of_chars + 1);
    }
    vsprintf_s(const_cast<char*>(temp.c_str()), num_of_chars + 1, format,
        marker);
    OutputDebugStringA(temp.c_str());
}

// 当使用预编译的头时，需要使用此源文件，编译才能成功。
