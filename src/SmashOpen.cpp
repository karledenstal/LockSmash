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
        auto* effectData = effect->baseEffect;

        if (bOnlyFF && spell->GetCastingType() != RE::MagicSystem::CastingType::kFireAndForget) {
            shouldUnlock = false;
            break;
        }

        shouldUnlock = bAllowAlteration && effectData->GetMagickSkill() == RE::ActorValue::kAlteration ||
                              bAllowShock && effectData->HasKeyword("MagicDamageShock") ||
                              GetSingleton()->MagicCanUnlock(effectData);
    }

    return shouldUnlock;
}

void SmashOpen::UnlockWithMagic(RE::TESObjectREFR* refr, RE::SpellItem* spell) {
    bool spellIsAllowed = GetSingleton()->IsAllowedToUnlock(spell);

    if (!spellIsAllowed) {
        RE::DebugNotification("This magic can not break this lock");
    } else {
        logger::info("This magic can unlock");
    }
}