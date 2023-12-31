﻿// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "head.h"

auto unload() -> void {
    hooks::unload();
    LOG("unload \n");
}
auto WatchExitThread(void *ctx) -> void {
    while (true) {
        if (GetAsyncKeyState(VK_END) & 1) {
            break;
        }
        Sleep(100);
    }
    global::Exit = true;
    unload();
}
auto init(void* ctx) -> bool {
    
    AllocConsole();
    SetConsoleTitleA("huoji debug console");
    freopen_s(reinterpret_cast<FILE**> stdout, "CONOUT$", "w", stdout);
    CreateThread(NULL, 0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(WatchExitThread),
        NULL, 0, NULL);
    uint64_t serverHandle{};
    uint64_t localizeHandle{};

    while (Offset::Module_tier0 == 0 || serverHandle == 0 || localizeHandle == 0)
    {
        if (global::Exit) {
            return false;
        }
        Offset::Module_tier0 = reinterpret_cast<uint64_t>(GetModuleHandleA("tier0"));
        serverHandle = reinterpret_cast<uint64_t>(GetModuleHandleA("server.dll"));
        localizeHandle = reinterpret_cast<uint64_t>(GetModuleHandleA("localize.dll"));

        Sleep(100);
    }
    global::IsMetaModInit = (GetModuleHandleA("metamod.2.cs2.dll") != nullptr);
    if (Offset::Init() == false) {
        LOG("Offset::Init() == false !\n");
        return false;
    }
    LOG("hacked by huoji 2023.9.14 \n");
    bool isSuccess = false;
    isSuccess = hooks::init();
    if (isSuccess) {
        LOG("plugin install success !\n");
        while (Offset::InitOffsetSuccess == false)
        {
            Sleep(200);
        }
        ScriptEngine::Init();
    }
    return isSuccess;
}
extern BOOL APIENTRY VersionHijack_DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved);
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
    bool result = true;
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            // 有vac
            //VersionHijack_DllMain(hModule, ul_reason_for_call, lpReserved);
            CreateThread(NULL, 0,
                reinterpret_cast<LPTHREAD_START_ROUTINE>(init),
                NULL, 0, NULL);
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return result;
}
