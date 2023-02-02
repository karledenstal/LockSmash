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

    ini.SaveFile(path);
}

bool Settings::IsBruteForceEnabled() {
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);

    bool IsEnabled = ini.GetBoolValue("BruteForceBasic", "bEnabled", true);

    if (IsEnabled) {
        Load();
    }

    return IsEnabled;
}

bool Settings::OnlyAllowBlunt() {
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);

    return ini.GetBoolValue("BruteForceBasic", "bOnlyBlunt", false);
}

bool Settings::OnlyAllowTwoHanded() {
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);

    return ini.GetBoolValue("BruteForceBasic", "bOnlyTwoHanded", false);
}

bool Settings::IsSkillRequirementEnabled() {
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);

    return ini.GetBoolValue("BruteForceBasic", "bEnableSkillRequirement", true);
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
        case RE::LOCK_LEVEL::kEasy: {
            auto easy = ini.GetDoubleValue("Skills", "fApprenticeSkill");
            logger::info("Easy lock: {}", easy);
            return easy;
        }
        case RE::LOCK_LEVEL::kAverage: {
            logger::info("average lock");
            return ini.GetDoubleValue("Skills", "fAdeptSkill");
        }
        case RE::LOCK_LEVEL::kHard: {
            logger::info("hard lock");
            return ini.GetDoubleValue("Skills", "fExpertSkill");
        }
        case RE::LOCK_LEVEL::kVeryHard: {
            logger::info("very hard lock");
            return ini.GetDoubleValue("Skills", "fMasterSkill");
        }
        default: {
            logger::info("very Easy lock");
            return ini.GetDoubleValue("Skills", "fNoviceSkill");
        }
    }
}

float Settings::GetSkillIncreaseAmount(const char* level) {
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);

    return static_cast<float>(ini.GetDoubleValue("Skills", level));
}

float Settings::GetForceMultiplier(const char* a_key) {
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);

    return static_cast<float>(ini.GetDoubleValue("Multipliers", a_key));
}
