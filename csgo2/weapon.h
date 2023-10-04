#pragma once
#include "head.h"
class CCSPlayerController;
namespace GameWeapons {

static const std::unordered_map<std::string, std::pair<std::string, int>>
    WeaponMap = {
        {"bizon", {"weapon_bizon", 1400}},
        {"mac10", {"weapon_mac10", 1400}},
        {"mp7", {"weapon_mp7", 1700}},
        {"mp9", {"weapon_mp9", 1250}},
        {"p90", {"weapon_p90", 2350}},
        {"ump45", {"weapon_ump45", 1700}},
        {"ak47", {"weapon_ak47", 2500}},
        {"aug", {"weapon_aug", 3500}},
        {"famas", {"weapon_famas", 2250}},
        {"galilar", {"weapon_galilar", 2000}},
        {"m4a4", {"weapon_m4a4", 3100}},
        {"m4a1_silencer", {"weapon_m4a1_silencer", 3100}},
        {"m4a1", {"weapon_m4a1_silencer", 3100}},
        {"a1", {"weapon_m4a1_silencer", 3100}},
        {"sg556", {"weapon_sg556", 3500}},
        {"awp", {"weapon_awp", 4750}},
        {"g3sg1", {"weapon_g3sg1", 5000}},
        {"scar20", {"weapon_scar20", 5000}},
        {"ssg08", {"weapon_ssg08", 2500}},
        {"mag7", {"weapon_mag7", 2000}},
        {"nova", {"weapon_nova", 1500}},
        {"sawedoff", {"weapon_sawedoff", 1500}},
        {"xm1014", {"weapon_xm1014", 3000}},
        {"m249", {"weapon_m249", 5750}},
        {"negev", {"weapon_negev", 5750}},
        {"deagle", {"weapon_deagle", 700}},
        {"elite", {"weapon_elite", 800}},
        {"fiveseven", {"weapon_fiveseven", 500}},
        {"glock", {"weapon_glock", 200}},
        {"hkp2000", {"weapon_hkp2000", 200}},
        {"p250", {"weapon_p250", 300}},
        {"tec9", {"weapon_tec9", 500}},
        {"usp_silencer", {"weapon_usp_silencer", 200}},
        {"cz75a", {"weapon_cz75a", 500}},
        {"revolver", {"weapon_revolver", 600}},
        {"kevlar", {"item_kevlar", 600}},
        {"he", {"weapon_hegrenade", 300}},
        {"molotov", {"weapon_hegrenade", 850}},
};
auto ParseWeaponCommand(CCSPlayerController* pController,
                        std::string pszWeaponName) -> bool;
};  // namespace GameWeapons
