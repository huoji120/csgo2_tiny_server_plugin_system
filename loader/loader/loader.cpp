// loader.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <windows.h>
#include <iostream>
#include <string.h>
#include <filesystem>
#include <fstream>
#include "../../raidjson/rapidjson/rapidjson.h"
#include <istreamwrapper.h>
#include <document.h>
struct _Config
{
    std::string path;
    std::string command;
};
namespace Config {
    std::vector<_Config> configs;
    auto ReadConfig() -> bool {
        bool result = false;
        std::ifstream ifs("config.json");
        if (!ifs.is_open()) {
            std::cerr << "Could not open file for reading!\n";
            return result;
        }

        rapidjson::IStreamWrapper isw(ifs);
        rapidjson::Document d;
        d.ParseStream(isw);

        const rapidjson::Value& servers = d["servers"]; // Using a reference for consecutive access is handy and faster.
        assert(servers.IsArray());
        for (rapidjson::SizeType i = 0; i < servers.Size(); i++) { // Uses SizeType instead of size_t
            const rapidjson::Value& server = servers[i];
            assert(server.IsObject()); // Each server should be an object.
            if (server.HasMember("path") && server["path"].IsString() && server.HasMember("command") && server["command"].IsString())
            {
                configs.push_back(_Config{
                    .path = server["path"].GetString(),
                    .command = server["command"].GetString()
                });
            }
        }
        result = true;
        return result;
    }
};

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
    Config::ReadConfig();
    if (Config::configs.size() > 0) {
        for (auto& config : Config::configs)
        {
            PROCESS_INFORMATION pi = createProcess(config.path, config.command);
            loadDll(pi);
        }
    }
    return 0;
}