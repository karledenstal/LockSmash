BruteMagic* BruteMagic::GetSingleton() {
    static BruteMagic singleton;
    return &singleton;
}

void BruteMagic::UnlockObject() {
    logger::info("Unlocking object");
}

float BruteMagic::GetSuccessChance(RE::SpellItem* spell, float fSkillReq) { 
    Settings* settings = Settings::GetSingleton();
    RE::TESNPC* player = RE::PlayerCharacter::GetSingleton()->GetActorBase();
    
    float fDestructionSkill = player->GetActorValue(RE::ActorValue::kDestruction);
    float fBaseValue = player->GetBaseActorValue(RE::ActorValue::kDestruction);
    float fMagicka = player->GetBaseActorValue(RE::ActorValue::kMagicka) / 25;
    float fSkillCalc = fDestructionSkill - static_cast<float>(fSkillReq);
    float fTwoHandedBonus = spell->IsTwoHanded() ? 5.0f : 0.0f;
    float fResult = fSkillCalc + fMagicka + fBaseValue + fTwoHandedBonus;

    float fMaxChance = settings->successChance.getMaxChance();
    float fMinChance = settings->successChance.getMinChance();

    if (fResult < 0.0f) {
        return fMinChance;
    } else if (fResult > 100.0f) {
        return fMaxChance;
    }

    return fResult;
}
