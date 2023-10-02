#include "tools.h"
namespace Tools {

    auto GetFiles(const std::string& path, std::vector<std::string>& files)
        -> void {
        std::string pattern(path);
        pattern.append("\\*");
        WIN32_FIND_DATAA data;
        HANDLE hFind;
        if ((hFind = FindFirstFileA(pattern.c_str(), &data)) !=
            INVALID_HANDLE_VALUE) {
            do {
                if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    if (strcmp(data.cFileName, ".") != 0 &&
                        strcmp(data.cFileName, "..") != 0) {
                        GetFiles((path + "\\" + data.cFileName), files);
                    }
                }
                else {
                    files.push_back(path + "\\" + data.cFileName);
                }
            } while (FindNextFileA(hFind, &data) != 0);
            FindClose(hFind);
        }
    }
    auto GetExeFileName() -> std::string {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        return std::string(buffer);
    }
    auto GetExePath() -> std::string {
        std::string f = GetExeFileName();
        return f.substr(0, f.find_last_of("\\/"));
    }
}