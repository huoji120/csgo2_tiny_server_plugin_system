#pragma once
#include "pch.h"

#define CALL_VIRTUAL(retType, idx, ...) \
    vmt::CallVirtual<retType>(idx, __VA_ARGS__)
namespace vmt {
    template <typename T = void*>
    inline T GetVMethod(uint32_t uIndex, void* pClass) {
        void** pVTable = *static_cast<void***>(pClass);
        return reinterpret_cast<T>(pVTable[uIndex]);
    }

    template <typename T, typename... Args>
    inline T CallVirtual(uint32_t uIndex, void* pClass, Args... args) {
        auto pFunc = GetVMethod<T(__thiscall*)(void*, Args...)>(uIndex, pClass);
        return pFunc(pClass, args...);
    }
}  // namespace vmt

