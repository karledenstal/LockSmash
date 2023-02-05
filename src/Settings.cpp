#include "Settings.h"

Settings* Settings::GetSingleton() {
    static Settings singleton;
    return std::addressof(singleton);
}

void Settings::LoadSettings() {
    constexpr auto path = L"Data/SKSE/Plugins/SmashOpenSKSE.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    ini.LoadFile(path);
    
    basic.Load(ini);
    magic.Load(ini);
    successChance.Load(ini);
    skills.Load(ini);
    multipliers.Load(ini);

    ini.SaveFile(path);
}

bool Settings::Basic::isEnabled() { return bEnabled; };
bool Settings::Basic::onlyAllowBlunt() { return bOnlyBlunt; };
bool Settings::Basic::onlyAllowTwoHanded() { return bOnlyTwoHanded; };
bool Settings::Basic::isSkillRequirementEnabled() { return bEnableSkillRequirement; };

bool Settings::Magic::isMagicEnabled() { return bEnableMagic; };
bool Settings::Magic::allowShockToUnlock() { return bAllowShockToUnlock; };
bool Settings::Magic::onlyFireAndForget() { return bOnlyFireAndForget; };
bool Settings::Magic::isConcentratedDamageDebuffEnabled() { return bConcentratedDamageDebuff; };
float Settings::Magic::getFrostLockBuff() { return fFrostedLockBuff; }
float Settings::Magic::getConcentratedDebuff() { return fConcentratedDamageDebuff; }

float Settings::SuccessChance::getMaxChance() { return fMaxChance; }
float Settings::SuccessChance::getMinChance() { return fMinChance; }

// Load & set all settings from ini file

void Settings::SuccessChance::Load(CSimpleIniA& a_ini) { 
    static const char* section = "SuccessChance";

    detail::config(a_ini, fMaxChance, section, "fMaxChance");
    detail::config(a_ini, fMinChance, section, "fMinChance");
}

void Settings::Magic::Load(CSimpleIniA& a_ini) {
    static const char* section = "Magic";

    detail::config(a_ini, bEnableMagic, section, "bEnableMagic");
    detail::config(a_ini, bAllowShockToUnlock, section, "bAllowShockToUnlock");
    detail::config(a_ini, bOnlyFireAndForget, section, "bOnlyFireAndForget");
    detail::config(a_ini, bConcentratedDamageDebuff, section, "bConcentratedDamageDebuff");

    detail::config(a_ini, fConcentratedDamageDebuff, section, "fConcentratedDamageDebuff");
    detail::config(a_ini, fFrostedLockBuff, section, "fFrostedLockBuff");
}

void Settings::Basic::Load(CSimpleIniA& a_ini) {
    static const char* section = "Basic";

    detail::config(a_ini, bEnabled, section, "bEnabled");
    detail::config(a_ini, bOnlyBlunt, section, "bOnlyBlunt");
    detail::config(a_ini, bOnlyTwoHanded, section, "bOnlyTwoHanded");
    detail::config(a_ini, bEnableSkillRequirement, section, "bEnableSkillRequirement");
}

void Settings::Multipliers::Load(CSimpleIniA& a_ini) {
    static const char* section = "Multipliers";

    detail::config(a_ini, fIron, section, "fIron");
    detail::config(a_ini, fSteel, section, "fSteel");
    detail::config(a_ini, fSilver, section, "fSilver");
    detail::config(a_ini, fImperial, section, "fImperial");
    detail::config(a_ini, fElven, section, "fElven");
    detail::config(a_ini, fDwarven, section, "fDwarven");
    detail::config(a_ini, fOrcish, section, "fOrcish");
    detail::config(a_ini, fNordic, section, "fNordic");
    detail::config(a_ini, fEbony, section, "fEbony");
    detail::config(a_ini, fStalhrim, section, "fStalhrim");
    detail::config(a_ini, fGlass, section, "fGlass");
    detail::config(a_ini, fDaedric, section, "fDaedric");
    detail::config(a_ini, fDragonbone, section, "fDragonbone");
}

void Settings::Skills::Load(CSimpleIniA& a_ini) {
    static const char* section = "Skills";

    detail::config(a_ini, fNoviceSkill, section, "fNoviceSkill");
    detail::config(a_ini, fNoviceSkillIncrease, section, "fNoviceSkillIncrease");
    detail::config(a_ini, fApprenticeSkill, section, "fApprenticeSkill");
    detail::config(a_ini, fApprenticeSkillIncrease, section, "fApprenticeSkillIncrease");
    detail::config(a_ini, fAdeptSkill, section, "fAdeptSkill");
    detail::config(a_ini, fAdeptSkillIncrease, section, "fAdeptSkillIncrease");
    detail::config(a_ini, fExpertSkill, section, "fExpertSkill");
    detail::config(a_ini, fExpertSkillIncrease, section, "fExpertSkillIncrease");
    detail::config(a_ini, fMasterSkill, section, "fMasterSkill");
    detail::config(a_ini, fMasterSkillIncrease, section, "fMasterSkillIncrease");
}