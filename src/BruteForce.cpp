BruteForce* BruteForce::GetSingleton() {
    static BruteForce singleton;
    return &singleton;
}

bool BruteForce::isCorrectWeaponType(RE::TESObjectWEAP* weapon, BruteForce::Unlock::WeaponType weaponType) {
    if (weaponType == Unlock::WeaponType::kBlunt) {
        return weapon->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>("_BF_BluntWeapons"), false);
    }
    
    if (weaponType == Unlock::WeaponType::kTwoHanded) {
        return weapon->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>("_BF_TwoHandedTypes"), false);
    }

    if (weaponType == Unlock::WeaponType::kWarhammer) {
        return weapon->HasKeywordString("WeapTypeWarhammer");
    }
    
    return false;
}

bool BruteForce::isCorrectMaterial(RE::TESObjectWEAP* weapon, std::string_view formList) {
    return weapon->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>(formList), false);
}

BruteForce::Unlock::Flag BruteForce::canUnlockSpecialized(RE::TESObjectWEAP* weapon, bool skillCheckPasses,
                                                          Unlock::WeaponType weaponType) { 
    bool isSpecializedWeapon = GetSingleton()->isCorrectWeaponType(weapon, weaponType);
    
    if (!isSpecializedWeapon) return Unlock::Flag::kWrongWeaponType;
    if (isSpecializedWeapon && !Settings::GetSingleton()->bruteForceBasic.isSkillRequirementEnabled())
        return Unlock::Flag::kPasses;

    if (isSpecializedWeapon && skillCheckPasses)
        return Unlock::Flag::kPasses;
    else if (isSpecializedWeapon && !skillCheckPasses)
        return Unlock::Flag::kSkillFail;
    else
        return Unlock::Flag::kFail;
}

BruteForce::Unlock::Flag BruteForce::canUnlockBasic(bool skillCheckPasses) {
    bool skillRequirementEnabled = Settings::GetSingleton()->bruteForceBasic.isSkillRequirementEnabled();
    if (skillRequirementEnabled && skillCheckPasses || !skillRequirementEnabled) {
        return Unlock::Flag::kPasses;
    }
    
    return Unlock::Flag::kSkillFail;
}

void BruteForce::IncreaseSkillExperience(RE::ActorValue SkillToIncrease, RE::LOCK_LEVEL lockLevel, RE::PlayerCharacter* Player) {
    Settings* Settings = Settings::GetSingleton();

    switch (lockLevel) {
        case RE::LOCK_LEVEL::kVeryEasy:
            Player->AddSkillExperience(SkillToIncrease, Settings->skills.fNoviceSkillIncrease);
            break;
        case RE::LOCK_LEVEL::kEasy:
            Player->AddSkillExperience(SkillToIncrease, Settings->skills.fApprenticeSkillIncrease);
            break;
        case RE::LOCK_LEVEL::kAverage:
            Player->AddSkillExperience(SkillToIncrease, Settings->skills.fAdeptSkillIncrease);
            break;
        case RE::LOCK_LEVEL::kHard:
            Player->AddSkillExperience(SkillToIncrease, Settings->skills.fExpertSkillIncrease);
            break;
        case RE::LOCK_LEVEL::kVeryHard:
            Player->AddSkillExperience(SkillToIncrease, Settings->skills.fMasterSkillIncrease);
            break;
        default:
            Player->AddSkillExperience(SkillToIncrease, 0);
            break;
    }
}

float BruteForce::GetWeaponMultiplier(RE::TESObjectWEAP* weapon) {
    Settings* Settings = Settings::GetSingleton();
    
    if (weapon->HasKeywordString("WeapMaterialSteel")) {
        return Settings->multipliers.fSteel;
    } else if (weapon->HasKeywordString("WeapMaterialImperial")) {
        return Settings->multipliers.fImperial;
    } else if (weapon->HasKeywordString("WeapMaterialSilver")) {
        return Settings->multipliers.fSilver;
    } else if (weapon->HasKeywordString("WeapMaterialDwarven")) {
        return Settings->multipliers.fDwarven;
    } else if (weapon->HasKeywordString("WeapMaterialElven")) {
        return Settings->multipliers.fElven;
    } else if (weapon->HasKeywordString("WeapMaterialOrcish")) {
        return Settings->multipliers.fOrcish;
    } else if (weapon->HasKeywordString("DLC2WeaponMaterialNordic")) {
        return Settings->multipliers.fNordic;
    } else if (weapon->HasKeywordString("WeapMaterialEbony")) {
        return Settings->multipliers.fEbony;
    } else if (weapon->HasKeywordString("DLC2WeaponMaterialStalhrim")) {
        return Settings->multipliers.fStalhrim;
    } else if (weapon->HasKeywordString("WeapMaterialDaedric")) {
        return Settings->multipliers.fDaedric;
    } else if (weapon->HasKeywordString("DLC1WeapMaterialDragonbone")) {
        return Settings->multipliers.fDragonbone;
    } else {
        return 0.0f;
    }
}

float BruteForce::GetSuccessChance(RE::TESObjectWEAP* weapon, RE::ActorValue skillUsed) {
    Settings* Settings = Settings::GetSingleton();
    RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
    float fWeaponForce = GetWeaponMultiplier(weapon);
    float fSkillReq = 25.0;
    float fWeaponSkill = player->GetActorBase()->GetActorValue(skillUsed);
    float fBaseValue = player->GetActorBase()->GetBaseActorValue(skillUsed);
    float fSkillCalc = fWeaponSkill - static_cast<float>(fSkillReq);
    float fStamina = player->GetActorBase()->GetBaseActorValue(RE::ActorValue::kStamina)/25;
    float fResult = fSkillCalc + fStamina + fBaseValue + static_cast<float>(fWeaponForce);

    float fMaxChance = Settings->successChance.getMaxChance();
    float fMinChance = Settings->successChance.getMinChance();

    logger::info("maxChance: {}", fMaxChance);

    logger::info("fResult: {}", fResult);

    if (fResult < 0.0f) {
        return fMinChance;
    } else if (fResult > 100.0f) {
        return fMaxChance;
    }
    
    return fResult;
}


