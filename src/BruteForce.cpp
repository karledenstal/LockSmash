BruteForce* BruteForce::GetSingleton() {
    static BruteForce singleton;
    return &singleton;
}

RE::BSEventNotifyControl BruteForce::ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*) {
    RE::TESObjectWEAP* attackSourceWeapon = RE::TESForm::LookupByID<RE::TESObjectWEAP>(event->source);
    RE::SpellItem* attackSourceMagic = RE::TESForm::LookupByID<RE::SpellItem>(event->source);

    if (attackSourceWeapon) {
         if (event->target->IsLocked() && event->cause->GetFormID() == 0x14) {
            logger::info("Target is locked");
            std::string_view formListId = GetFormList(event->target);
            
            if (event->target->GetLockLevel() == RE::LOCK_LEVEL::kRequiresKey) {
                RE::DebugNotification("This lock is sealed by Fate");
            } else {
                HitThatLock(event->target->As<RE::TESObjectREFR>(), attackSourceWeapon, formListId);
            }
        } else {
            logger::trace("Target is not an object");
        }
    }
    return RE::BSEventNotifyControl::kContinue;
}

void BruteForce::UnlockObject(RE::TESObjectREFR* refr, bool IsTwoHanded) {
    logger::info("Unlocking object");
    RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

    refr->GetLock()->SetLocked(false);
    RE::PlaySound("NPCHumanWoodChopSD");

    IncreaseSkillExperience(IsTwoHanded ? RE::ActorValue::kTwoHanded : RE::ActorValue::kOneHanded, refr->GetLockLevel(), player);

    bool IsAContainer = refr->HasContainer();

    if (refr->GetActorOwner() != player->GetActorBase() && IsAContainer) {
        logger::info("Is a container");
        player->StealAlarm(refr, refr->GetBaseObject(), 0, 0, refr->GetActorOwner(), true);
        logger::info("This is not owned by the player!!");
    } else if (refr->GetActorOwner() != player->GetActorBase() && !IsAContainer) {
        logger::info("This is not a container");
        player->TrespassAlarm(refr, refr->GetActorOwner(), 25);
    } else {
        logger::info("This is owned by the player");
    }
};

void BruteForce::HitThatLock(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, std::string_view formList) {
    if (weapon->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>(formList), false)) {
        Settings* settings = Settings::GetSingleton();
        bool onlyBlunt = settings->OnlyAllowBlunt();
        bool onlyTwoHanded = settings->OnlyAllowTwoHanded();
        bool IsSkillARequirement = settings->IsSkillRequirementEnabled();
        RE::LOCK_LEVEL lockLevel = refr->GetLockLevel();
        RE::TESNPC* player = RE::PlayerCharacter::GetSingleton()->GetActorBase();

        bool IsTwoHanded = weapon->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>("_BF_TwoHandedTypes"), false);

        double skillReq = settings->GetLockSkillReq(lockLevel);
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
            UnlockBasedOnMaterial(refr, IsTwoHanded, IsSkillARequirement, skillLevel >= skillReq);
        }
    } else {
        RE::DebugNotification("This lock is too sturdy for this weapon");
    }
}

std::string_view BruteForce::GetFormList(RE::TESObjectREFRPtr refr) {
    RE::LOCK_LEVEL lockLevel = refr->GetLockLevel();

	if (lockLevel == RE::LOCK_LEVEL::kEasy) {
		return "_BF_EasyLockMaterials";
	} else if (lockLevel == RE::LOCK_LEVEL::kAverage) {
		return "_BF_AverageLockMaterials";
	} else if (lockLevel == RE::LOCK_LEVEL::kHard) {
		return "_BF_HardLockMaterials";
	} else if (lockLevel == RE::LOCK_LEVEL::kVeryHard) {
		return "_BF_VeryHardLockMaterials";
    } else if (lockLevel == RE::LOCK_LEVEL::kVeryEasy) {
        return "_BF_VeryEasyLockMaterials";
    } else {
        return "";
    }
}

void BruteForce::UnlockWithTwoHandedOnly(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, bool PlayerSkillMatches,
                                         bool IsUsingSkillRequirement) {
    bool IsWeaponTwoHanded = weapon->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>("_BF_TwoHandedTypes"), false);

    if (IsUsingSkillRequirement) {
        if (IsWeaponTwoHanded && PlayerSkillMatches) {
            UnlockObject(refr, true);
        } else if (IsWeaponTwoHanded && !PlayerSkillMatches) {
            RE::DebugNotification("I'm not strong enough to break this lock");
        } else if (!IsWeaponTwoHanded) {
            RE::DebugNotification("Only a two handed weapon could break this lock");
        } else {
            RE::DebugNotification("This lock won't budge");
        }
    } else {
        if (IsWeaponTwoHanded) {
            UnlockObject(refr, true);
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
            UnlockObject(refr, IsWeaponTwoHanded);
        } else if (IsBluntWeapon && !PlayerSkillMatches) {
            RE::DebugNotification("I'm not strong enough to break this lock");
        } else if (!IsBluntWeapon) {
            RE::DebugNotification("Only a blunt weapon could break this lock");
        } else {
            RE::DebugNotification("This lock won't budge");
        }
    } else {
        if (IsBluntWeapon) {
            UnlockObject(refr, IsWeaponTwoHanded);
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
            UnlockObject(refr, true);
        } else if (IsWeaponWarhammer && !PlayerSkillMatches) {
            RE::DebugNotification("I'm not strong enough to break this lock");
        } else if (!IsWeaponWarhammer) {
            RE::DebugNotification("Only a warhammer could break this lock");
        } else {
            RE::DebugNotification("This lock won't budge");
        }
    } else {
        if (IsWeaponWarhammer) {
            UnlockObject(refr, true);
        } else {
            RE::DebugNotification("Only a warhammer could break this lock");
        }
    }
}

void BruteForce::UnlockBasedOnMaterial(RE::TESObjectREFR* refr, bool IsWeaponTwoHanded, bool IsUsingSkillRequirement,
                                       bool PlayerSkillMatches) {
    if (IsUsingSkillRequirement && PlayerSkillMatches || !IsUsingSkillRequirement) {
        UnlockObject(refr, IsWeaponTwoHanded);
    } else {
        RE::DebugNotification("I'm not strong enough to break this lock");
    }
}

void BruteForce::IncreaseSkillExperience(RE::ActorValue SkillToIncrease, RE::LOCK_LEVEL lockLevel, RE::PlayerCharacter* Player) {
    Settings* Settings = Settings::GetSingleton();

    switch (lockLevel) {
        case RE::LOCK_LEVEL::kVeryEasy:
            Player->AddSkillExperience(SkillToIncrease, Settings->GetSkillIncreaseAmount("fNoviceSkillIncrease"));
            break;
        case RE::LOCK_LEVEL::kEasy:
            Player->AddSkillExperience(SkillToIncrease, Settings->GetSkillIncreaseAmount("fApprenticeSkillIncrease"));
            break;
        case RE::LOCK_LEVEL::kAverage:
            Player->AddSkillExperience(SkillToIncrease, Settings->GetSkillIncreaseAmount("fAdeptSkillIncrease"));
            break;
        case RE::LOCK_LEVEL::kHard:
            Player->AddSkillExperience(SkillToIncrease, Settings->GetSkillIncreaseAmount("fExpertSkillIncrease"));
            break;
        case RE::LOCK_LEVEL::kVeryHard:
            Player->AddSkillExperience(SkillToIncrease, Settings->GetSkillIncreaseAmount("fMasterSkillIncrease"));
            break;
        default:
            Player->AddSkillExperience(SkillToIncrease, 0);
            break;
    }
}

//float BruteForce::NormalizeValue(float afValue, float afMin, float afMax) { return ((afValue - afMax) / (afMax - afMin)) + 1.0; }
//
//float BruteForce::GetSuccessChance(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, RE::ActorValue SkillUsed) {
//    Settings* Settings = Settings::GetSingleton();
//    RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
//    float fLockDifficulty = 0.0f;
//    double fWeaponForce = 0.0;
//
//    if (weapon->HasKeywordString("WeapMaterialSteel")) {
//        fWeaponForce = Settings->GetForceMultiplier("fSteel");
//    } else if (weapon->HasKeywordString("WeapMaterialImperial")) {
//        fWeaponForce = Settings->GetForceMultiplier("fImperial");
//    } else if (weapon->HasKeywordString("WeapMaterialSilver")) {
//        fWeaponForce = Settings->GetForceMultiplier("fSilver");
//    } else if (weapon->HasKeywordString("WeapMaterialDwarven")) {
//        fWeaponForce = Settings->GetForceMultiplier("fDwarven");
//    } else if (weapon->HasKeywordString("WeapMaterialElven")) {
//        fWeaponForce = Settings->GetForceMultiplier("fElven");
//    } else if (weapon->HasKeywordString("WeapMaterialOrcish")) {
//        fWeaponForce = Settings->GetForceMultiplier("fOrcish");
//    } else if (weapon->HasKeywordString("DLC2WeaponMaterialNordic")) {
//        fWeaponForce = Settings->GetForceMultiplier("´fNordic");
//    } else if (weapon->HasKeywordString("WeapMaterialEbony")) {
//        fWeaponForce = Settings->GetForceMultiplier("fEbony");
//    } else if (weapon->HasKeywordString("DLC2WeaponMaterialStalhrim")) {
//        fWeaponForce = Settings->GetForceMultiplier("fStalhrim");
//    } else if (weapon->HasKeywordString("WeapMaterialDaedric")) {
//        fWeaponForce = Settings->GetForceMultiplier("fDaedric");
//    } else if (weapon->HasKeywordString("DLC1WeapMaterialDragonbone")) {
//        fWeaponForce = Settings->GetForceMultiplier("fDragonbone");
//    } else {
//        fWeaponForce = 0.0;
//    }
//       
//    switch (refr->GetLockLevel()) { 
//        case RE::LOCK_LEVEL::kVeryEasy:
//            fLockDifficulty = 0.5f;
//            break;
//        case RE::LOCK_LEVEL::kEasy:
//            fLockDifficulty = 1.0f;
//            break;
//        case RE::LOCK_LEVEL::kAverage:
//            fLockDifficulty = 2.0f;
//            break;
//        case RE::LOCK_LEVEL::kHard:
//            fLockDifficulty = 3.0f;
//            break;
//        case RE::LOCK_LEVEL::kVeryHard:
//            fLockDifficulty = 4.0f;
//            break;
//        case RE::LOCK_LEVEL::kRequiresKey:
//            fLockDifficulty = 5.0f;
//            break;
//        default:
//            fLockDifficulty = -1.0f;
//            break;
//    }
//    
//    double fSkillReq = Settings->GetLockSkillReq(refr->GetLockLevel());
//    float fPlayerNorm = NormalizeValue(player->GetLevel(), 1.0f, 81.0f);
//    float fLuck = 1.0f + static_cast<float>(rand()) * static_cast<float>(3.0f - 1.0f) / RAND_MAX;
//    float fWeaponSkill = player->GetActorBase()->GetActorValue(SkillUsed);
//    float fBaseValue = player->GetActorBase()->GetBaseActorValue(SkillUsed);
//    float fPlayerForce = fPlayerNorm * 0.45;
//    float fForceValue = (fWeaponForce * 0.35f) + (fWeaponSkill * 0.2f) + fPlayerForce;
//    float fSuccessFactor = (((fPlayerForce + fLuck) / 2) * 50 < rand() * 100);
//    float fResult = (fLockDifficulty) - (fForceValue * fLuck * fPlayerForce);
//
//    float fLockHealth = (fLockDifficulty + (fSkillReq * 0.01)) - (fBaseValue * 0.1); // 5.85
//    float fPlayerDmg = (fWeaponForce * 10) + (fWeaponSkill * 0.1) + fLuck;
//    
//    return 0.0f;
//}


