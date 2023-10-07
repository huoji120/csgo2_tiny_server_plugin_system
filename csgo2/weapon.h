#pragma once
#include "head.h"
class CCSPlayerController;
namespace GameWeapons {

static const std::unordered_map<std::string, std::pair<std::string, int>>
    WeaponMap = {
        {"bizon", {"weapon_bizon", 26}},
        {"mac10", {"weapon_mac10", 27}},
        {"mp7", {"weapon_mp7", 23}},
        {"mp9", {"weapon_mp9", 34}},
        {"p90", {"weapon_p90", 19}},
        {"ump45", {"weapon_ump45", 24}},
        {"ak47", {"weapon_ak47", 7}},
        {"aug", {"weapon_aug", 8}},
        {"famas", {"weapon_famas", 10}},
        {"galilar", {"weapon_galilar", 13}},
        {"m4a4", {"weapon_m4a1", 16}},
        {"m4a1", {"weapon_m4a1_silencer", 60}},
        {"sg556", {"weapon_sg556", 39}},
        {"awp", {"weapon_awp", 9}},
        {"g3sg1", {"weapon_g3sg1", 11}},
        {"scar20", {"weapon_scar20", 38}},
        {"ssg08", {"weapon_ssg08", 40}},
        {"mag7", {"weapon_mag7", 29}},
        {"nova", {"weapon_nova", 35}},
        {"sawedoff", {"weapon_sawedoff", 29}},
        {"xm1014", {"weapon_xm1014", 25}},
        {"m249", {"weapon_m249", 14}},
        {"negev", {"weapon_negev", 28}},
        {"deagle", {"weapon_deagle", 1}},
        {"elite", {"weapon_elite", 2}},
        {"fiveseven", {"weapon_fiveseven", 3}},
        {"glock", {"weapon_glock", 4}},
        {"hkp2000", {"weapon_hkp2000", 32}},
        {"p250", {"weapon_p250", 36}},
        {"tec9", {"weapon_tec9", 30}},
        {"usp_silencer", {"weapon_usp_silencer", 61}},
        {"cz75a", {"weapon_cz75a", 63}},
        {"revolver", {"weapon_revolver", 64}},
        {"he", {"weapon_hegrenade", 44}},
        {"molotov", {"weapon_molotov", 46}},
        {"knife", {"weapon_knife", 42}},  // default CT knife
        {"kevlar", {"item_kevlar", 50}},
};
auto ParseWeaponCommand(CCSPlayerController* pController,
                        std::string pszWeaponName) -> bool;
};  // namespace GameWeapons
