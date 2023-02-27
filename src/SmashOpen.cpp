#include "Settings.h"
#include "SmashOpen.hpp"

SmashOpen* SmashOpen::GetSingleton() {
    static SmashOpen singleton;
    return &singleton;
}

RE::BSEventNotifyControl SmashOpen::ProcessEvent(const RE::TESHitEvent* event,
                                                 RE::BSTEventSource<RE::TESHitEvent>*){
    Settings* settings = Settings::GetSingleton();

    if (event && event->target && event->source && event->cause && settings && settings->basic.isEnabled()) {
        logger::info("SmashOpenSKSE: Enabled");
        auto* sourceAsWeapon = RE::TESForm::LookupByID(event->source)->As<RE::TESObjectWEAP>();
        auto* sourceAsMagic = RE::TESForm::LookupByID(event->source)->As<RE::SpellItem>();

        if (sourceAsMagic && !sourceAsWeapon && settings->magic.isMagicEnabled()) {
            logger::info("Attacks with spell: {}", sourceAsMagic->GetName());
            GetSingleton()->UnlockWithMagic(event->target->As<RE::TESObjectREFR>(), sourceAsMagic);
        }

        if (!sourceAsMagic && sourceAsWeapon) {
            logger::info("Attacks with weapon: {}", sourceAsWeapon->GetName());
        }

        if (event->target->GetLockLevel() == RE::LOCK_LEVEL::kRequiresKey) {
            RE::DebugNotification("This is sealed by Fate");
        }
    }
    
    return RE::BSEventNotifyControl::kContinue;
}

bool SmashOpen::MagicCanUnlock(RE::EffectSetting* effect) {
    return effect->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>("_BF_AllowedMagic"), false);
}

bool SmashOpen::IsAllowedToUnlock(RE::SpellItem* spell) {
    Settings* settings = Settings::GetSingleton();
    bool bAllowShock = settings->magic.allowShockToUnlock();
    bool bAllowAlteration = settings->magic.allowAlteration();
    bool bOnlyFF = settings->magic.onlyFireAndForget();
    bool shouldUnlock = false;

    for (auto*& effect : spell->effects) {
        bool correctKeyword = bAllowShock && effect->baseEffect->HasKeyword("MagicDamageShock") ||
                              GetSingleton()->MagicCanUnlock(effect->baseEffect);

        if (correctKeyword) {
            shouldUnlock = true;
            break;
        }
    }

    if (!shouldUnlock && bAllowAlteration && spell->GetAssociatedSkill() == RE::ActorValue::kAlteration) {
        shouldUnlock = true;
    }

    if (shouldUnlock && bOnlyFF && spell->GetCastingType() != RE::MagicSystem::CastingType::kFireAndForget) {
        shouldUnlock = false;
    }

    return shouldUnlock;
}

void SmashOpen::UnlockIt(RE::REFR_LOCK* lock) {
    lock->SetLocked(false);
    RE::PlaySound("NPCHumanWoodChopSD");
}

void SmashOpen::UnlockWithMagic(RE::TESObjectREFR* refr, RE::SpellItem* spell) {
    Settings* settings = Settings::GetSingleton();
    bool skillReqEnabled = settings->basic.isSkillRequirementEnabled();
    bool spellIsAllowed = GetSingleton()->IsAllowedToUnlock(spell);

    if (!spellIsAllowed) {
        RE::DebugNotification("This magic can not break this lock");
    } else {
        logger::info("This magic can unlock");

        if (skillReqEnabled) {
            // Run & get success chance to calculate unlocks
            logger::info("Run & get success chance to calculate unlocks");
        } else {
            // If skill is not a requirement, just unlock it
            GetSingleton()->UnlockIt(refr->GetLock());
        }
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

float SmashOpen::GetMagicSuccessChance(RE::SpellItem* spellUsed, RE::LOCK_LEVEL lockLevel) {
    Settings* settings = Settings::GetSingleton();
    RE::TESNPC* player = RE::PlayerCharacter::GetSingleton()->GetActorBase();

    bool concentrateSpellDebuff = settings->magic.isConcentratedDamageDebuffEnabled() &&
                                  spellUsed->GetCastingType() == RE::MagicSystem::CastingType::kConcentration;
    float fConcentratedDebuff = concentrateSpellDebuff ? settings->magic.getConcentratedDebuff() : 0.0f;
    float fSkillReq = GetSingleton()->GetSkillReq(lockLevel);
    float fAssocSkill = player->GetActorValue(spellUsed->GetAssociatedSkill());
    float fBaseAssocSkill = player->GetBaseActorValue(spellUsed->GetAssociatedSkill());
    float fMagicka = player->GetBaseActorValue(RE::ActorValue::kMagicka) / 5;
    float fSkillCalc = fAssocSkill - static_cast<float>(fSkillReq);
    float fTwoHandedBonus = spellUsed->IsTwoHanded() ? 5.0f : 0.0f;
    float fResult = (fSkillCalc + fMagicka + fTwoHandedBonus + fBaseAssocSkill) - fConcentratedDebuff;

     if (fResult < 0.0f) {
        return settings->successChance.getMinChance();
    } else if (fResult > 100.0f) {
        return settings->successChance.getMaxChance();
    }

    return fResult;
}