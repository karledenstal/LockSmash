#include "Settings.h"

Settings* Settings::GetSingleton() {
    static Settings singleton;
    return std::addressof(singleton);
}

void Settings::Load() {
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);

    // Brute Force
    bruteForceBasic.Load(ini);

    // Skills
    skills.Load(ini);

    // Multipliers
    multipliers.Load(ini);

    ini.SaveFile(path);
}

bool Settings::IsBruteForceEnabled() {
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);

    if (ini.GetBoolValue("BruteForce", "bEnabled", true)) {
        Load();
    }

    return ini.GetBoolValue("BruteForce", "bEnabled", true);
}

bool Settings::OnlyAllowBlunt() {
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);

    return ini.GetBoolValue("BruteForce", "bOnlyBlunt", false);
}

bool Settings::OnlyAllowTwoHanded() {
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);

    return ini.GetBoolValue("BruteForce", "bOnlyTwoHanded", false);
}

bool Settings::IsSkillRequirementEnabled() {
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);

    return ini.GetBoolValue("BruteForce", "bEnableSkillRequirement", true);
}

double Settings::GetLockSkillReq(RE::LOCK_LEVEL lockLevel) { 
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path); 

    switch (lockLevel) { 
        case RE::LOCK_LEVEL::kRequiresKey:
        case RE::LOCK_LEVEL::kUnlocked:
            return 900.0;
        case RE::LOCK_LEVEL::kEasy:
            return ini.GetDoubleValue("Skills", "iApprenticeSkill");
        case RE::LOCK_LEVEL::kAverage:
            return ini.GetDoubleValue("Skills", "iAdeptSkill");
        case RE::LOCK_LEVEL::kHard:
            return ini.GetDoubleValue("Skills", "iExpertSkill");
        case RE::LOCK_LEVEL::kVeryHard:
            return ini.GetDoubleValue("Skills", "iMasterSkill");
        default:
            return ini.GetDoubleValue("Skills", "iNoviceSkill");

    }
}

double Settings::GetForceMultiplier(BruteForce::WEAP_MATERIAL material) {
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);

    switch (material) { 
        case BruteForce::kIron:
            return ini.GetDoubleValue("Multipliers", "fIron");
        case BruteForce::kSteel:
            return ini.GetDoubleValue("Multipliers", "fSteel");
        case BruteForce::kSilver:
            return ini.GetDoubleValue("Multipliers", "fSilver");
        case BruteForce::kImperial:
            return ini.GetDoubleValue("Multipliers", "fIron");
        case BruteForce::kElven:
            return ini.GetDoubleValue("Multipliers", "fElven");
        case BruteForce::kDwarven:
            return ini.GetDoubleValue("Multipliers", "fDwarven");
        case BruteForce::kOrcish:
            return ini.GetDoubleValue("Multipliers", "fOrcish");
        case BruteForce::kNordic:
            return ini.GetDoubleValue("Multipliers", "fNordic");
        case BruteForce::kEbony:
            return ini.GetDoubleValue("Multipliers", "fEbony");
        case BruteForce::kStalhrim:
            return ini.GetDoubleValue("Multipliers", "fStalhrim");
        case BruteForce::kDaedric:
            return ini.GetDoubleValue("Multipliers", "fDaedric");
        case BruteForce::kDragonbone:
            return ini.GetDoubleValue("Multipliers", "fDragonbone");
        default:
            return 0.0;
    }
}

void Settings::BruteForceBasic::Load(CSimpleIniA& a_ini) {
    static const char* section = "BruteForceBasic";

    logger::info("BruteForceLoad");

    detail::get_value(a_ini, bEnabled, section, "Brute Force Enabled", ";Enables the mod if set to true");
    detail::get_value(a_ini, bEnableSkillRequirement, section, "Enable Skill Requirement", ";Takes skill into consideration");
    detail::get_value(a_ini, bOnlyBlunt, section, "Allow only blunt weapons", ";Allows only blunt weapons to break locks");
    detail::get_value(a_ini, bOnlyTwoHanded, section, "Allow only two handed weapons", ";Allows only two handed weapons to break locks");
}

void Settings::Skills::Load(CSimpleIniA& a_ini) {
    static const char* section = "Skills";

    logger::info("SkillsLoad");

    detail::get_value(a_ini, iNoviceSkill, section, "Novice skill requirement", ";Skill required to break open novice locks");
    detail::get_value(a_ini, iApprenticeSkill, section, "Apprentice skill requirement",
                      ";Skill required to break open apprentice locks");
    detail::get_value(a_ini, iAdeptSkill, section, "Adept skill requirement",
                      ";Skill required to break open adept locks");
    detail::get_value(a_ini, iExpertSkill, section, "Expert skill requirement",
                      ";Skill required to break open expert locks");
    detail::get_value(a_ini, iMasterSkill, section, "Master skill requirement",
                      ";Skill required to break open master locks");
}

void Settings::Multipliers::Load(CSimpleIniA& a_ini) {
    static const char* section = "Multipliers";

    logger::info("MultipliersLoad");

    detail::get_value(a_ini, fIron, section, "Iron force multiplier",
                      ";Force multiplier for iron weapons");
    detail::get_value(a_ini, fSteel, section, "Steel force multiplier", ";Force multiplier for steel weapons");
    detail::get_value(a_ini, fSilver, section, "Silver force multiplier", ";Force multiplier for silver weapons");
    detail::get_value(a_ini, fImperial, section, "Imperial force multiplier", ";Force multiplier for imperial weapons");
    detail::get_value(a_ini, fElven, section, "Elven force multiplier", ";Force multiplier for elven weapons");
    detail::get_value(a_ini, fDwarven, section, "Dwarven force multiplier", ";Force multiplier for dwarven weapons");
    detail::get_value(a_ini, fOrcish, section, "Orcish force multiplier", ";Force multiplier for orcish weapons");
    detail::get_value(a_ini, fNordic, section, "Nordic force multiplier", ";Force multiplier for nordic weapons");
    detail::get_value(a_ini, fEbony, section, "Ebony force multiplier", ";Force multiplier for ebony weapons");
    detail::get_value(a_ini, fStalhrim, section, "Stahlrim force multiplier", ";Force multiplier for stahlrim weapons");
    detail::get_value(a_ini, fGlass, section, "Glass force multiplier", ";Force multiplier for glass weapons");
    detail::get_value(a_ini, fDaedric, section, "Daedric force multiplier", ";Force multiplier for daedric weapons");
    detail::get_value(a_ini, fDragonbone, section, "Dragonbone force multiplier", ";Force multiplier for dragonbone weapons");
}