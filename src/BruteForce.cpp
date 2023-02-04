BruteForce* BruteForce::GetSingleton() {
    static BruteForce singleton;
    return &singleton;
}

RE::BSEventNotifyControl BruteForce::ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*) {
    if (event && event->target && event->source && event->cause) {
        logger::info("Events are all initialized");
        RE::TESObjectWEAP* attackSourceWeapon = RE::TESForm::LookupByID<RE::TESObjectWEAP>(event->source);

        if (attackSourceWeapon) {
             if (BruteBase::GetSingleton()->IsTargetLocked(event)) {
                logger::info("Target is locked");
                 std::string_view formListId = BruteBase::GetSingleton()->GetFormList(event->target->GetLockLevel());
            
                if (event->target->GetLockLevel() == RE::LOCK_LEVEL::kRequiresKey) {
                    RE::DebugNotification("This lock is sealed by Fate");
                } else {
                    HitThatLock(event->target->As<RE::TESObjectREFR>(), attackSourceWeapon, formListId);
                    return RE::BSEventNotifyControl::kContinue;
                }
            } else {
                logger::trace("Target is not locked");
                return RE::BSEventNotifyControl::kStop;
            }
        } else {
            logger::trace("Source is not a weapon");
            return RE::BSEventNotifyControl::kStop;
        }
    }

    return RE::BSEventNotifyControl::kStop;
}

void BruteForce::UnlockObject(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, bool IsTwoHanded) {
    RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
    auto skillUsed = IsTwoHanded ? RE::ActorValue::kTwoHanded : RE::ActorValue::kOneHanded;
    auto fChanceOfSuccess = GetSuccessChance(refr, weapon, skillUsed);

    if ((rand() % 100) < fChanceOfSuccess) {
        BruteBase::GetSingleton()->UnlockedTarget(refr, player);
        IncreaseSkillExperience(skillUsed, refr->GetLockLevel(), player);
    } else {
        RE::DebugNotification("This lock is too difficult");
    }
    
    BruteBase::GetSingleton()->CreateDetection(refr, player);
};

void BruteForce::HitThatLock(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, std::string_view formList) {
    if (weapon->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>(formList), false)) {
        Settings::BruteForceBasic settings = Settings::GetSingleton()->bruteForceBasic;
        bool onlyBlunt = settings.onlyAllowBlunt();
        bool onlyTwoHanded = settings.onlyAllowTwoHanded();
        bool IsSkillARequirement = settings.isSkillRequirementEnabled();

        RE::LOCK_LEVEL lockLevel = refr->GetLockLevel();
        RE::TESNPC* player = RE::PlayerCharacter::GetSingleton()->GetActorBase();

        bool IsTwoHanded = weapon->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>("_BF_TwoHandedTypes"), false);

        double skillReq = BruteBase::GetSingleton()->GetSkillRequirement(lockLevel);
        float skillLevel = player->GetActorValue(IsTwoHanded ? RE::ActorValue::kTwoHanded : RE::ActorValue::kOneHanded);

        if (onlyTwoHanded && onlyBlunt) {
            UnlockWithBluntAndTwoHanded(refr, weapon, skillLevel >= skillReq, IsSkillARequirement);
        } else if (onlyTwoHanded) {
            UnlockWithTwoHandedOnly(refr, weapon, skillLevel >= skillReq, IsSkillARequirement);
        } else if (onlyBlunt) {
            UnlockWithBluntOnly(refr, weapon, skillLevel >= skillReq, IsSkillARequirement, IsTwoHanded);
        } else if (weapon->GetWeaponType() == RE::WEAPON_TYPE::kBow ||
                   weapon->GetWeaponType() == RE::WEAPON_TYPE::kCrossbow) {
            RE::DebugNotification("I can't destroy this lock with a bow");
        } else {
            UnlockBasedOnMaterial(refr, weapon, IsTwoHanded, IsSkillARequirement, skillLevel >= skillReq);
        }
    } else {
        RE::DebugNotification("This lock is too sturdy for this weapon");
    }
}

void BruteForce::UnlockWithTwoHandedOnly(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, bool PlayerSkillMatches,
                                         bool IsUsingSkillRequirement) {
    bool IsWeaponTwoHanded = weapon->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>("_BF_TwoHandedTypes"), false);

    if (IsUsingSkillRequirement) {
        if (IsWeaponTwoHanded && PlayerSkillMatches) {
            UnlockObject(refr, weapon, true);
        } else if (IsWeaponTwoHanded && !PlayerSkillMatches) {
            RE::DebugNotification("I'm not strong enough to break this lock");
        } else if (!IsWeaponTwoHanded) {
            RE::DebugNotification("Only a two handed weapon could break this lock");
        } else {
            RE::DebugNotification("This lock won't budge");
        }
    } else {
        if (IsWeaponTwoHanded) {
            UnlockObject(refr, weapon, true);
        } else {
            RE::DebugNotification("Only a two handed weapon could break this lock");
        }
    }
}

void BruteForce::UnlockWithBluntOnly(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, bool PlayerSkillMatches,
                                     bool IsUsingSkillRequirement, bool IsWeaponTwoHanded) {
    bool IsBluntWeapon = weapon->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>("_BF_BluntWeapons"), false);

    if (IsUsingSkillRequirement) {
        if (IsBluntWeapon && PlayerSkillMatches) {
            UnlockObject(refr, weapon, IsWeaponTwoHanded);
        } else if (IsBluntWeapon && !PlayerSkillMatches) {
            RE::DebugNotification("I'm not strong enough to break this lock");
        } else if (!IsBluntWeapon) {
            RE::DebugNotification("Only a blunt weapon could break this lock");
        } else {
            RE::DebugNotification("This lock won't budge");
        }
    } else {
        if (IsBluntWeapon) {
            UnlockObject(refr, weapon, IsWeaponTwoHanded);
        } else {
            RE::DebugNotification("Only a blunt weapon could break this lock");
        }
    }
}

void BruteForce::UnlockWithBluntAndTwoHanded(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon,
                                             bool PlayerSkillMatches,
                                             bool IsUsingSkillRequirement) {
    bool IsWeaponWarhammer = weapon->HasKeywordString("WeapTypeWarhammer");
    if (IsUsingSkillRequirement) {
        if (IsWeaponWarhammer && PlayerSkillMatches) {
            UnlockObject(refr, weapon, true);
        } else if (IsWeaponWarhammer && !PlayerSkillMatches) {
            RE::DebugNotification("I'm not strong enough to break this lock");
        } else if (!IsWeaponWarhammer) {
            RE::DebugNotification("Only a warhammer could break this lock");
        } else {
            RE::DebugNotification("This lock won't budge");
        }
    } else {
        if (IsWeaponWarhammer) {
            UnlockObject(refr, weapon, true);
        } else {
            RE::DebugNotification("Only a warhammer could break this lock");
        }
    }
}

void BruteForce::UnlockBasedOnMaterial(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, bool IsWeaponTwoHanded,
                                       bool IsUsingSkillRequirement,
                                       bool PlayerSkillMatches) {
    if (IsUsingSkillRequirement && PlayerSkillMatches || !IsUsingSkillRequirement) {
        UnlockObject(refr, weapon, IsWeaponTwoHanded);
    } else {
        RE::DebugNotification("I'm not strong enough to break this lock");
    }
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

float BruteForce::GetSuccessChance(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, RE::ActorValue SkillUsed) {
    Settings* Settings = Settings::GetSingleton();
    RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
    float fWeaponForce = GetWeaponMultiplier(weapon);
    float fSkillReq = BruteBase::GetSingleton()->GetSkillRequirement(refr->GetLockLevel());
    float fWeaponSkill = player->GetActorBase()->GetActorValue(SkillUsed);
    float fBaseValue = player->GetActorBase()->GetBaseActorValue(SkillUsed);
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


