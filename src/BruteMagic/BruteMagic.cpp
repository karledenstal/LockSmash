BruteMagic* BruteMagic::GetSingleton() {
    static BruteMagic singleton;
    return &singleton;
}

void BruteMagic::UnlockObject() {
    logger::info("Unlocking object");
}

bool BruteMagic::isAllowedMagic(RE::SpellItem* spell) {
    auto& effects = spell->effects;
    bool hasAllowedEffect = false;
    bool allowShock = Settings::GetSingleton()->magic.allowShockToUnlock();
    
    for (auto& effect : effects) {
        auto& effectData = effect->baseEffect;
        
        bool correctKeyword = effectData->HasKeyword("MagicDamageFire") ||
                              effectData->HasKeyword("MagicDamageFrost") ||
                              allowShock && effectData->HasKeyword("MagicDamageShock");
        
        if (correctKeyword) {
            hasAllowedEffect = true;
            break;
        }
    }
    
    return hasAllowedEffect && spell->GetSpellType() == RE::MagicSystem::SpellType::kSpell;
}

BruteMagic::Unlock::Flag BruteMagic::canUnlock(bool skillCheckPasses, RE::MagicSystem::CastingType castingType) {
    bool skillRequirementEnabled = Settings::GetSingleton()->bruteForceBasic.isSkillRequirementEnabled();
    bool allowOnlyFF = Settings::GetSingleton()->magic.onlyFireAndForget();
    
    // user only allows Fire and Forget spells && the spel they cast is NOT fire and forget
    if (allowOnlyFF && castingType != RE::MagicSystem::CastingType::kFireAndForget)
        return Unlock::Flag::kWrongCastingType;
    
    if (skillRequirementEnabled && !skillCheckPasses)
        return Unlock::Flag::kSkillFailed;

    if (skillRequirementEnabled && skillCheckPasses) return Unlock::Flag::kPasses;

    return Unlock::Flag::kFail;
}

void BruteMagic::IncreaseMagicSkill(RE::PlayerCharacter* player, RE::LOCK_LEVEL lockLevel) {
    Settings* Settings = Settings::GetSingleton();

    switch (lockLevel) {
        case RE::LOCK_LEVEL::kVeryEasy:
            player->AddSkillExperience(RE::ActorValue::kDestruction, Settings->skills.fNoviceSkillIncrease);
            break;
        case RE::LOCK_LEVEL::kEasy:
            player->AddSkillExperience(RE::ActorValue::kDestruction, Settings->skills.fApprenticeSkillIncrease);
            break;
        case RE::LOCK_LEVEL::kAverage:
            player->AddSkillExperience(RE::ActorValue::kDestruction, Settings->skills.fAdeptSkillIncrease);
            break;
        case RE::LOCK_LEVEL::kHard:
            player->AddSkillExperience(RE::ActorValue::kDestruction, Settings->skills.fExpertSkillIncrease);
            break;
        case RE::LOCK_LEVEL::kVeryHard:
            player->AddSkillExperience(RE::ActorValue::kDestruction, Settings->skills.fMasterSkillIncrease);
            break;
        default:
            player->AddSkillExperience(RE::ActorValue::kDestruction, 0);
    }
}

float BruteMagic::GetSuccessChance(RE::SpellItem* spell, float fSkillReq) { 
    Settings* settings = Settings::GetSingleton();
    RE::TESNPC* player = RE::PlayerCharacter::GetSingleton()->GetActorBase();
    
    bool concentrateSpellDebuff = settings->magic.isConcentratedDamageDebuffEnabled();
    float fConcentratedDebuff = concentrateSpellDebuff && spell->GetCastingType() == RE::MagicSystem::CastingType::kConcentration ? 10.0f : 0.0f;
    float fDestructionSkill = player->GetActorValue(RE::ActorValue::kDestruction);
    float fAlterationSkill = player->GetActorValue(RE::ActorValue::kAlteration);
    float fRestorationSkill = player->GetActorValue(RE::ActorValue::kRestoration);
    float fConjurationSkill = player->GetActorValue(RE::ActorValue::kConjuration);
    float fIllusionSkill = player->GetActorValue(RE::ActorValue::kIllusion);
    float fMiddleMagicSkill = (fAlterationSkill + fRestorationSkill + fConjurationSkill + fIllusionSkill + fDestructionSkill) / 5;
    float fBaseValue = player->GetBaseActorValue(RE::ActorValue::kDestruction);
    float fMagicka = player->GetBaseActorValue(RE::ActorValue::kMagicka) / 25;
    float fSkillCalc = fDestructionSkill - static_cast<float>(fSkillReq);
    float fTwoHandedBonus = spell->IsTwoHanded() ? 5.0f : 0.0f;
    float fResult = fSkillCalc + fMagicka + fBaseValue + fMiddleMagicSkill + fTwoHandedBonus - fConcentratedDebuff;
    
    logger::info("fResult {}", fResult);

    if (fResult < 0.0f) {
        return settings->successChance.getMaxChance();
    } else if (fResult > 100.0f) {
        return settings->successChance.getMinChance();
    }

    return fResult;
}
