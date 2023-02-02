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

float Settings::GetLockSkillReq(RE::LOCK_LEVEL lockLevel) { 
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);

    switch (lockLevel) { 
        case RE::LOCK_LEVEL::kRequiresKey:
        case RE::LOCK_LEVEL::kUnlocked:
            return 900.0;
        case RE::LOCK_LEVEL::kEasy:
            return static_cast<float>(ini.GetDoubleValue("Skills", "iApprenticeSkill"));
        case RE::LOCK_LEVEL::kAverage:
            return static_cast<float>(ini.GetDoubleValue("Skills", "iAdeptSkill"));
        case RE::LOCK_LEVEL::kHard:
            return static_cast<float>(ini.GetDoubleValue("Skills", "iExpertSkill"));
        case RE::LOCK_LEVEL::kVeryHard:
            return static_cast<float>(ini.GetDoubleValue("Skills", "iMasterSkill"));
        default:
            return static_cast<float>(ini.GetDoubleValue("Skills", "iNoviceSkill"));

    }
}

float Settings::GetForceMultiplier(BruteForce::WEAP_MATERIAL material) {
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);

    switch (material) { 
        case BruteForce::kIron:
            return static_cast<float>(ini.GetDoubleValue("Multipliers", "fIron"));
        case BruteForce::kSteel:
            return static_cast<float>(ini.GetDoubleValue("Multipliers", "fSteel"));
        case BruteForce::kSilver:
            return static_cast<float>(ini.GetDoubleValue("Multipliers", "fSilver"));
        case BruteForce::kImperial:
            return static_cast<float>(ini.GetDoubleValue("Multipliers", "fIron"));
        case BruteForce::kElven:
            return static_cast<float>(ini.GetDoubleValue("Multipliers", "fElven"));
        case BruteForce::kDwarven:
            return static_cast<float>(ini.GetDoubleValue("Multipliers", "fDwarven"));
        case BruteForce::kOrcish:
            return static_cast<float>(ini.GetDoubleValue("Multipliers", "fOrcish"));
        case BruteForce::kNordic:
            return static_cast<float>(ini.GetDoubleValue("Multipliers", "fNordic"));
        case BruteForce::kEbony:
            return static_cast<float>(ini.GetDoubleValue("Multipliers", "fEbony"));
        case BruteForce::kStalhrim:
            return static_cast<float>(ini.GetDoubleValue("Multipliers", "fStalhrim"));
        case BruteForce::kDaedric:
            return static_cast<float>(ini.GetDoubleValue("Multipliers", "fDaedric"));
        case BruteForce::kDragonbone:
            return static_cast<float>(ini.GetDoubleValue("Multipliers", "fDragonbone"));
        default:
            return 0.0;
    }
}

void Settings::BruteForceBasic::Load(CSimpleIniA& a_ini) {
    static const char* section = "BruteForceBasic";

    logger::info("BruteForceLoad");

    detail::get_value(a_ini, bEnabled, section, "bEnabled", ";Enables the mod if set to true");
    detail::get_value(a_ini, bEnableSkillRequirement, section, "bEnableSkillRequirement", ";Takes skill into consideration");
    detail::get_value(a_ini, bOnlyBlunt, section, "bOnlyBlunt", ";Allows only blunt weapons to break locks");
    detail::get_value(a_ini, bOnlyTwoHanded, section, "bOnlyTwoHanded", ";Allows only two handed weapons to break locks");
}

void Settings::Skills::Load(CSimpleIniA& a_ini) {
    static const char* section = "Skills";

    logger::info("SkillsLoad");

    detail::get_value(a_ini, fNoviceSkill, section, "fNoviceSkill", ";Skill required to break open novice locks");
    detail::get_value(a_ini, fApprenticeSkill, section, "fApprenticeSkill",
                      ";Skill required to break open apprentice locks");
    detail::get_value(a_ini, fAdeptSkill, section, "fAdeptSkill",
                      ";Skill required to break open adept locks");
    detail::get_value(a_ini, fExpertSkill, section, "fExpertSkill",
                      ";Skill required to break open expert locks");
    detail::get_value(a_ini, fMasterSkill, section, "fMasterSkill",
                      ";Skill required to break open master locks");
}

void Settings::Multipliers::Load(CSimpleIniA& a_ini) {
    static const char* section = "Multipliers";

    logger::info("MultipliersLoad");

    detail::get_value(a_ini, fIron, section, "fIron",
                      ";Force multiplier for iron weapons");
    detail::get_value(a_ini, fSteel, section, "fSteel", ";Force multiplier for steel weapons");
    detail::get_value(a_ini, fSilver, section, "fSilver", ";Force multiplier for silver weapons");
    detail::get_value(a_ini, fImperial, section, "fImperial", ";Force multiplier for imperial weapons");
    detail::get_value(a_ini, fElven, section, "fElven", ";Force multiplier for elven weapons");
    detail::get_value(a_ini, fDwarven, section, "fDwarven", ";Force multiplier for dwarven weapons");
    detail::get_value(a_ini, fOrcish, section, "fOrcish", ";Force multiplier for orcish weapons");
    detail::get_value(a_ini, fNordic, section, "fNordic", ";Force multiplier for nordic weapons");
    detail::get_value(a_ini, fEbony, section, "fEbony", ";Force multiplier for ebony weapons");
    detail::get_value(a_ini, fStalhrim, section, "fStalhrim", ";Force multiplier for stahlrim weapons");
    detail::get_value(a_ini, fGlass, section, "fGlass", ";Force multiplier for glass weapons");
    detail::get_value(a_ini, fDaedric, section, "fDaedric", ";Force multiplier for daedric weapons");
    detail::get_value(a_ini, fDragonbone, section, "fDragonbone", ";Force multiplier for dragonbone weapons");
}