#include "memory.h"
namespace Memory {
    auto PathVscript() -> void {
        CModule vscript_old("vscript_old.dll");
        CModule vscript("vscript.dll");

        uint64_t vscriptPathAddr = 0;
        if (vscript_old.IsLoaded() == true) {
            vscript_old.FindPattern(Offset::pattern_VscriptPath).Get(vscriptPathAddr);
        }
        else {
            vscript.FindPattern(Offset::pattern_VscriptPath).Get(vscriptPathAddr);
        }
        if (vscriptPathAddr != 0) {
            const static char PatchVScriptEnable[] = { 0xBE, 0x02 };
            DWORD oldProtect;
            VirtualProtect(reinterpret_cast<void*>(vscriptPathAddr), sizeof(PatchVScriptEnable), PAGE_EXECUTE_READWRITE, &oldProtect);
            memcpy(reinterpret_cast<void*>(vscriptPathAddr), PatchVScriptEnable, sizeof(PatchVScriptEnable));
            VirtualProtect(reinterpret_cast<void*>(vscriptPathAddr), sizeof(PatchVScriptEnable), oldProtect, &oldProtect);
            LOG("success patch vscript at %llx \n", vscriptPathAddr);
        }
    }
};