#include "tools.h"
namespace Tools {
auto toLower(const std::string& str) -> std::string{
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}
auto GetDirs(const std::string& path)
    -> std::pair<std::vector<std::string>, std::vector<std::string>> {
    std::vector<std::string> dirPaths;
    std::vector<std::string> dirNames;
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
                    dirPaths.push_back(path + "\\" + data.cFileName);
                    dirNames.push_back(data.cFileName);
                    auto subDirs = GetDirs(path + "\\" + data.cFileName);
                    dirPaths.insert(dirPaths.end(), subDirs.first.begin(),
                                    subDirs.first.end());
                    dirNames.insert(dirNames.end(), subDirs.second.begin(),
                                    subDirs.second.end());
                }
            }
        } while (FindNextFileA(hFind, &data) != 0);
        FindClose(hFind);
    }
    return {dirPaths, dirNames};
}
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
            } else {
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
}  // namespace Tools
