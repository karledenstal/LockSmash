BruteMagic* BruteMagic::GetSingleton() {
    static BruteMagic singleton;
    return &singleton;
}

RE::BSEventNotifyControl BruteMagic::ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*) {
    RE::SpellItem* attackSourceMagic = RE::TESForm::LookupByID<RE::SpellItem>(event->source);
    
    if (attackSourceMagic) {
        if (event->target->IsLocked() && event->cause->GetFormID() == 0x14) {
            bool allowsShock = Settings::GetSingleton()->magic.allowShockToUnlock();
            RE::MagicSystem::SpellType spellType = attackSourceMagic->GetSpellType();
            RE::ActorValue associatedSkill = attackSourceMagic->GetAssociatedSkill();
            bool allowedMagic = attackSourceMagic->HasKeywordString("MagicDamageFire") ||
                                attackSourceMagic->HasKeywordString("MagicDamageFrost") ||
                                allowsShock && attackSourceMagic->HasKeywordString("MagicDamageShock");
            logger::info("Attacking with magic: {}", attackSourceMagic->GetAssociatedSkill());
        }
    }

    return RE::BSEventNotifyControl::kContinue;
};

void BruteMagic::UnlockObject() {
    logger::info("Unlocking object");
}

float BruteMagic::GetSuccessChance() { return 0.0f; }
