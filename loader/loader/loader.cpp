// loader.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <windows.h>
#include <iostream>
#include <string.h>
#include <filesystem>

// 读取INI文件
std::pair<std::string, std::string> readConfig() {
    char path[255], command[255];

    GetPrivateProfileStringA("server", "path", "", path, 255, ".\\config.ini");
    GetPrivateProfileStringA("server", "command", "", command, 255, ".\\config.ini");

    return { std::string(path), std::string(command) };
}

// 创建进程
PROCESS_INFORMATION createProcess(const std::string& path, const std::string& command) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // 注意：CreateProcessW需要宽字符，如果你的项目是使用多字节字符集，需要转换
    if (!CreateProcessA(path.c_str(), const_cast<char*>(command.c_str()), NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed (" << GetLastError() << ").\n";
    }

    return pi;
}

// 加载DLL
void loadDll(PROCESS_INFORMATION pi) {
    LPVOID addr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    LPVOID arg = (LPVOID)VirtualAllocEx(pi.hProcess, NULL, strlen("csgo2.dll") + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    WriteProcessMemory(pi.hProcess, arg, "csgo2.dll", strlen("csgo2.dll") + 1, NULL);
    HANDLE hThread = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)addr, arg, 0, NULL);

    if (hThread == NULL) {
        std::cerr << "CreateRemoteThread failed (" << GetLastError() << ").\n";
    }

    ResumeThread(pi.hThread);
}

int main() {
    auto [path, command] = readConfig();
    PROCESS_INFORMATION pi = createProcess(path, command);
    loadDll(pi);

    return 0;
}