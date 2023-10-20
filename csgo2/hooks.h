#pragma once
#include "head.h"
class IRecipientFilter;
typedef bool(__fastcall* FireEventServerSide_t)(CGameEventManager*, IGameEvent*,
                                                bool);
typedef void(__fastcall* Host_Say_t)(void*, void*, bool, int, const char*);
typedef void(__fastcall* OnClientConnect_t)(void*, CPlayerSlot, const char*,
                                            uint64_t, const char*, const char*,
                                            bool);
typedef void(__fastcall* OnClientDisconnect_t)(void*, CPlayerSlot, int,
                                               const char*, uint64_t,
                                               const char*);
typedef void(__fastcall* StartupServer_t)(void*,
                                          const GameSessionConfiguration_t&,
                                          ISource2WorldSession*, const char*);
typedef void(__fastcall* GameFrame_t)(void*, bool, bool, bool);
typedef void(__fastcall* UTIL_SayText2Filter_t)(IRecipientFilter&,
                                                CCSPlayerController*, uint64_t,
                                                const char*, const char*,
                                                const char*, const char*,
                                                const char*);
//typedef void(__fastcall* PostEventAbstract_t)(void*, CSplitScreenSlot, bool, IRecipientFilter*, INetworkSerializable*, const void*, unsigned long);
typedef void(__fastcall* PostEventAbstract_t)(void*, CSplitScreenSlot, bool, int, const uint64_t*, INetworkSerializable*, const void*, unsigned long, NetChannelBufType_t);
namespace hooks {
extern Host_Say_t original_Host_Say;
auto init() -> bool;
auto unload() -> void;
}  // namespace hooks
