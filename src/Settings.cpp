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
    bruteForce.Load(ini);

    // Skills
    skills.Load(ini);

    // Multipliers
    multipliers.Load(ini);

    ini.SaveFile(path);
}

bool Settings::isBruteForceEnabled() {
    constexpr auto path = L"Data/SKSE/Plugins/BruteForce.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);

    if (ini.GetBoolValue("BruteForce", "bEnabled", true)) {
        Load();
    }

    return ini.GetBoolValue("BruteForce", "bEnabled", true);
}

void Settings::BruteForce::Load(CSimpleIniA& a_ini) {
    static const char* section = "BruteForce";

    logger::info("BruteForceLoad");
}

void Settings::Skills::Load(CSimpleIniA& a_ini) {
    static const char* section = "Skills";

    logger::info("SkillsLoad");
}

void Settings::Multipliers::Load(CSimpleIniA& a_ini) {
    static const char* section = "Multipliers";

    logger::info("MultipliersLoad");
}