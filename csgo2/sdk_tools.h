#pragma once
#include "head.h"
inline int PlayerSlot_to_EntityIndex(int PlayerSlot) { return PlayerSlot + 1; }
inline int EntityIndex_to_PlayerSlot(int EntityIndex) {
    return EntityIndex - 1;
}
#define HUD_PRINTNOTIFY		1
#define HUD_PRINTCONSOLE	2
#define HUD_PRINTTALK		3
#define HUD_PRINTCENTER		4
enum class _ChatType { kTeam, kAll, kConsole };
enum class _HubType { kNotify = 1, kConsole, kTalk, kCenter, kMax };

namespace SdkTools {
auto ProcessChatString(const std::string& input)
    -> std::tuple<bool, _ChatType, std::string>;
auto SentChatToClient(CCSPlayerController* player, _HubType hubtype, const char* msg, ...) -> void;
auto SendConsoleChat(_HubType hubtype, const char* msg, ...) -> void;
};  // namespace SdkTools
