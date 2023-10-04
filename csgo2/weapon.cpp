#include "weapon.h"
namespace GameWeapons {
auto ParseWeaponCommand(CCSPlayerController* pController,
                        std::string pszWeaponName) -> bool {
    if (WeaponMap.find(pszWeaponName) == WeaponMap.end()) {
        return false;
    }
    const auto [fullWeaponName, weaponPirce] = WeaponMap.at(pszWeaponName);
    auto pItemServices = reinterpret_cast<void*>(
        pController->m_hPawn().Get<CCSPlayerPawn>()->m_pItemServices());
    if (pItemServices == NULL) {
        return false;
    }
    Offset::FnGiveNamedItem(pItemServices, fullWeaponName.c_str(), nullptr,
                            nullptr, nullptr, nullptr);
    return true;
}
};  // namespace GameWeapons
