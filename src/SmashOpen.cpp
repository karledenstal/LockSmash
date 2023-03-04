#include "Settings.h"
#include "SmashOpen.hpp"

SmashOpen* SmashOpen::GetSingleton() {
    static SmashOpen singleton;
    return &singleton;
}

bool SmashOpen::LockMagicProps::getLockProps(SmashOpen::LockMagicProps::LockProp prop) {
    switch (prop) { 
    case SmashOpen::LockMagicProps::LockProp::kBurning:
        return SmashOpen::GetSingleton()->lockMagicProps.lockIsBurning;
    case SmashOpen::LockMagicProps::LockProp::kShocked:
        return SmashOpen::GetSingleton()->lockMagicProps.lockIsShocked;
    default:
        return SmashOpen::GetSingleton()->lockMagicProps.lockIsFrosted;
    }
}

RE::BSEventNotifyControl SmashOpen::ProcessEvent(const RE::TESHitEvent* event,
                                                 RE::BSTEventSource<RE::TESHitEvent>*){
    if (Initialize(event)) {
        logger::info("SmashOpenSKSE: Enabled");

        // unlock that shit
        Unlock(event->target, event->source);

        if (event->target->GetLockLevel() == RE::LOCK_LEVEL::kRequiresKey) {
            RE::DebugNotification("This is sealed by Fate");
        }
    }
    
    return RE::BSEventNotifyControl::kContinue;
}

bool SmashOpen::Initialize(const RE::TESHitEvent* event) { 
    Settings* settings = Settings::GetSingleton(); 
    return event && event->target && event->source && event->cause && settings && settings->basic.isEnabled() &&
           event->target->IsLocked() && event->cause->GetFormID() == 0x14;
}

bool SmashOpen::MagicCanUnlock(RE::EffectSetting* effect) {
    return effect->HasKeyword("MagicDamageFrost") || effect->HasKeyword("MagicDamageFire");
}

bool SmashOpen::IsSpellAllowedToUnlock(RE::SpellItem* spell) {
    Settings* settings = Settings::GetSingleton();
    bool bAllowShock = settings->magic.allowShockToUnlock();
    bool bAllowAlteration = settings->magic.allowAlteration();
    bool bOnlyFF = settings->magic.onlyFireAndForget();
    bool shouldUnlock = false;

    for (auto*& effect : spell->effects) {
        bool correctKeyword = bAllowShock && effect->baseEffect->HasKeyword("MagicDamageShock") ||
                              MagicCanUnlock(effect->baseEffect);

        if (correctKeyword) {
            shouldUnlock = true;
            break;
        }
    }

    if (bOnlyFF && spell->GetCastingType() != RE::MagicSystem::CastingType::kFireAndForget) {
        return false;
    }

    if (bAllowAlteration && spell->GetAssociatedSkill() == RE::ActorValue::kAlteration || shouldUnlock) {
        return true;
    }

    return shouldUnlock;
}

void SmashOpen::UnlockIt(RE::TESObjectREFRPtr target) {
    // unlock that motherfrickin lock
    target->GetLock()->SetLocked(false);
    // play some shitty sound
    RE::PlaySound("NPCHumanWoodChopSD");
    CreateDetectionEvent();
}

void SmashOpen::CreateDetectionEvent() { logger::info("Create a detection event somehow??"); }

void SmashOpen::Unlock(RE::TESObjectREFRPtr target, RE::FormID source) {
    Settings* settings = Settings::GetSingleton();
    RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
    bool skillReqEnabled = settings->basic.isSkillRequirementEnabled();
    
    if (skillReqEnabled) {
        auto* sourceAs = RE::TESForm::LookupByID(source);
        float fHitCalc = 0.0f;
        auto skillToIncrease = RE::ActorValue::kNone;

        if (sourceAs->GetFormType() == RE::FormType::Weapon) {
            fHitCalc = CalculateWeaponSuccessChance(sourceAs->As<RE::TESObjectWEAP>(), target->GetLockLevel());
            skillToIncrease = GetAssociatedSkill(sourceAs->As<RE::TESObjectWEAP>());
        } else if (sourceAs->GetFormType() == RE::FormType::Spell) {
            bool spellIsAllowed = IsSpellAllowedToUnlock(sourceAs->As<RE::SpellItem>());
            fHitCalc = spellIsAllowed
                           ? CalculateSpellSuccessChance(sourceAs->As<RE::SpellItem>(), target->GetLockLevel())
                           : 0.0f;
            skillToIncrease = sourceAs->As<RE::SpellItem>()->GetAssociatedSkill();
        }

       // if fHitCalc is smaller than 0, then return minChance value
        if (fHitCalc < 0.0f) {
            fHitCalc = settings->successChance.getMinChance();
        } else if (fHitCalc > 100.0f) {
            fHitCalc = settings->successChance.getMaxChance();
        }

        if ((rand() % 101) < fHitCalc) {
            UnlockIt(target);
            player->AddSkillExperience(skillToIncrease, GetSkillIncrease(target->GetLockLevel()));
        } else {
            RE::DebugNotification("This lock won't budge");
        }
    } else {
        // If skill is not a requirement, just unlock it
        UnlockIt(target);
    }
}

float SmashOpen::GetSkillReq(RE::LOCK_LEVEL lockLevel) {
    Settings* settings = Settings::GetSingleton();
    switch (lockLevel) {
        case RE::LOCK_LEVEL::kEasy:
            return settings->skills.fApprenticeSkill;
        case RE::LOCK_LEVEL::kAverage:
            return settings->skills.fAdeptSkill;
        case RE::LOCK_LEVEL::kHard:
            return settings->skills.fExpertSkill;
        case RE::LOCK_LEVEL::kVeryHard:
            return settings->skills.fMasterSkill;
        case RE::LOCK_LEVEL::kRequiresKey:
            return 900.0f;
        default:
            return settings->skills.fNoviceSkill;
    }
}

RE::ActorValue SmashOpen::GetAssociatedSkill(RE::TESObjectWEAP* weapon) {
    if (weapon->IsHandToHandMelee()) {
        return RE::ActorValue::kHeavyArmor;
    }

    if (weapon->IsTwoHandedAxe() || weapon->IsTwoHandedSword()) {
        return RE::ActorValue::kTwoHanded;
    }

    return RE::ActorValue::kOneHanded;
}

float SmashOpen::GetSkillIncrease(RE::LOCK_LEVEL lockLevel) {
    Settings* settings = Settings::GetSingleton();

    switch (lockLevel) {
        case RE::LOCK_LEVEL::kVeryEasy:
            return settings->skills.fNoviceSkillIncrease;
        case RE::LOCK_LEVEL::kEasy:
            return settings->skills.fApprenticeSkillIncrease;
        case RE::LOCK_LEVEL::kAverage:
            return settings->skills.fAdeptSkillIncrease;
        case RE::LOCK_LEVEL::kHard:
            return settings->skills.fExpertSkillIncrease;
        case RE::LOCK_LEVEL::kVeryHard:
            return settings->skills.fMasterSkillIncrease;
        default:
            return 0.0f;
    }
}

float SmashOpen::CalculateSpellSuccessChance(RE::SpellItem* spellUsed, RE::LOCK_LEVEL lockLevel) {
    Settings* settings = Settings::GetSingleton();
    RE::TESNPC* player = RE::PlayerCharacter::GetSingleton()->GetActorBase();

    bool concentrateSpellDebuff = settings->magic.isConcentratedDamageDebuffEnabled() &&
                                  spellUsed->GetCastingType() == RE::MagicSystem::CastingType::kConcentration;
    float fConcentratedDebuff = concentrateSpellDebuff ? settings->magic.getConcentratedDebuff() : 0.0f;
    float fSkillReq = GetSkillReq(lockLevel);
    float fAssocSkill = player->GetActorValue(spellUsed->GetAssociatedSkill());
    float fBaseAssocSkill = player->GetBaseActorValue(spellUsed->GetAssociatedSkill());
    float fMagicka = player->GetBaseActorValue(RE::ActorValue::kMagicka) / 5;
    float fSkillCalc = fAssocSkill - static_cast<float>(fSkillReq);
    float fTwoHandedBonus = spellUsed->IsTwoHanded() ? 5.0f : 0.0f;

    return (fSkillCalc + fMagicka + fTwoHandedBonus + fBaseAssocSkill) - fConcentratedDebuff;
}

float SmashOpen::CalculateWeaponSuccessChance(RE::TESObjectWEAP* weaponUsed, RE::LOCK_LEVEL lockLevel) {
    Settings* settings = Settings::GetSingleton();
    auto* player = RE::PlayerCharacter::GetSingleton()->GetActorBase();

    float fWeaponDmg = weaponUsed->GetAttackDamage();
    float fSkillReq = static_cast<float>(GetSkillReq(lockLevel));
    float fWeaponSkill = player->GetActorValue(GetAssociatedSkill(weaponUsed));
    float fStamina = player->GetActorValue(RE::ActorValue::kStamina) / 50;
    float fFrostLockDebuff = lockMagicProps.getLockProps(SmashOpen::LockMagicProps::LockProp::kFrosted)
                                 ? settings->magic.getFrostLockBuff()
                                 : 0.0f;

    return (fWeaponSkill - fSkillReq) + (fWeaponDmg / 2) + fFrostLockDebuff + fStamina;
}
