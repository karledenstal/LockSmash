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
