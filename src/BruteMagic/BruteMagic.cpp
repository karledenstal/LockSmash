#include "BruteMagic.h"

BruteMagic* BruteMagic::GetSingleton() {
    static BruteMagic singleton;
    return &singleton;
}

bool BruteMagic::getIsAllowedType(RE::MagicSystem::SpellType a_type) {
    return a_type == RE::MagicSystem::SpellType::kSpell || a_type == RE::MagicSystem::SpellType::kScroll ||
           a_type == RE::MagicSystem::SpellType::kLeveledSpell || a_type == RE::MagicSystem::SpellType::kVoicePower;
}

bool BruteMagic::LockMagicProps::getLockIsFrosted() { return LockMagicProps::lockIsFrosted; }
void BruteMagic::LockMagicProps::setLockIsFrosted(bool a_bool) { LockMagicProps::lockIsFrosted = a_bool; }

bool BruteMagic::isAllowedMagic(RE::SpellItem* spell) {
    auto& effects = spell->effects;
    bool hasAllowedEffect = false;
    bool allowShock = Settings::GetSingleton()->magic.allowShockToUnlock();
    bool allowedTypes = GetSingleton()->getIsAllowedType(spell->GetSpellType());
    
    for (auto& effect : effects) {
        auto& effectData = effect->baseEffect;
        bool correctKeyword = false;
        bool isInList = effectData->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>("_BF_AllowedMagic"), false);
        
        if (allowShock) {
            correctKeyword = effectData->HasKeyword("MagicDamageShock") || isInList;
        } else {
            correctKeyword = isInList;
        }

        if (effectData->HasKeyword("MagicDamageFrost")) {
            auto inst = GetSingleton();
            inst->lockMagicProps.setLockIsFrosted(true);
        }

        if (correctKeyword) {
            hasAllowedEffect = true;
            break;
        }
    }
    
    return hasAllowedEffect && allowedTypes;
}

BruteMagic::Unlock::Flag BruteMagic::canUnlock(bool skillCheckPasses, RE::MagicSystem::CastingType castingType) {
    bool skillRequirementEnabled = Settings::GetSingleton()->basic.isSkillRequirementEnabled();
    bool allowOnlyFF = Settings::GetSingleton()->magic.onlyFireAndForget();
    
    // user only allows Fire and Forget spells && the spel they cast is NOT fire and forget
    if (allowOnlyFF && castingType != RE::MagicSystem::CastingType::kFireAndForget)
        return Unlock::Flag::kWrongCastingType;
    
    if (skillRequirementEnabled && !skillCheckPasses)
        return Unlock::Flag::kSkillFailed;

    if (skillRequirementEnabled && skillCheckPasses) return Unlock::Flag::kPasses;

    return Unlock::Flag::kFail;
}

void BruteMagic::IncreaseMagicSkill(RE::ActorValue skillUsed, RE::PlayerCharacter* player, RE::LOCK_LEVEL lockLevel) {
    Settings* Settings = Settings::GetSingleton();

    if (skillUsed == RE::ActorValue::kNone) {
        player->AddSkillExperience(RE::ActorValue::kDestruction, 0);
        return;
    } 
    
    switch (lockLevel) {
        case RE::LOCK_LEVEL::kVeryEasy:
            player->AddSkillExperience(skillUsed, Settings->skills.fNoviceSkillIncrease);
            break;
        case RE::LOCK_LEVEL::kEasy:
            player->AddSkillExperience(skillUsed, Settings->skills.fApprenticeSkillIncrease);
            break;
        case RE::LOCK_LEVEL::kAverage:
            player->AddSkillExperience(skillUsed, Settings->skills.fAdeptSkillIncrease);
            break;
        case RE::LOCK_LEVEL::kHard:
            player->AddSkillExperience(skillUsed, Settings->skills.fExpertSkillIncrease);
            break;
        case RE::LOCK_LEVEL::kVeryHard:
            player->AddSkillExperience(skillUsed, Settings->skills.fMasterSkillIncrease);
            break;
        default:
            player->AddSkillExperience(skillUsed, 0);
    }
}

float BruteMagic::GetSuccessChance(RE::SpellItem* spell, float fSkillReq) { 
    Settings* settings = Settings::GetSingleton();
    RE::TESNPC* player = RE::PlayerCharacter::GetSingleton()->GetActorBase();
    
    bool concentrateSpellDebuff = settings->magic.isConcentratedDamageDebuffEnabled() &&
                                  spell->GetCastingType() == RE::MagicSystem::CastingType::kConcentration;
    float fConcentratedDebuff = concentrateSpellDebuff ? settings->magic.getConcentratedDebuff() : 0.0f;
    float fDestructionSkill = player->GetActorValue(RE::ActorValue::kDestruction);
    float fAlterationSkill = player->GetActorValue(RE::ActorValue::kAlteration);
    float fRestorationSkill = player->GetActorValue(RE::ActorValue::kRestoration);
    float fConjurationSkill = player->GetActorValue(RE::ActorValue::kConjuration);
    float fIllusionSkill = player->GetActorValue(RE::ActorValue::kIllusion);
    float fMiddleMagicSkill = (fAlterationSkill + fRestorationSkill + fConjurationSkill + fIllusionSkill + fDestructionSkill) / 5;
    float fBaseValue = player->GetBaseActorValue(RE::ActorValue::kDestruction);
    float fMagicka = player->GetBaseActorValue(RE::ActorValue::kMagicka) / 16;
    float fSkillCalc = fDestructionSkill - static_cast<float>(fSkillReq);
    float fTwoHandedBonus = spell->IsTwoHanded() ? 5.0f : 0.0f;
    float fResult = fSkillCalc + fMagicka + fBaseValue + fMiddleMagicSkill + fTwoHandedBonus - fConcentratedDebuff;
    
    logger::info("fResult {}", fResult);

    if (fResult < 0.0f) {
        return settings->successChance.getMinChance();
    } else if (fResult > 100.0f) {
        return settings->successChance.getMaxChance();
    }

    return fResult;
}
