#include "BruteForce.h"
#include "Settings.h"

BruteForce* BruteForce::GetSingleton() {
    static BruteForce singleton;
    return &singleton;
}

RE::BSEventNotifyControl BruteForce::ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*) {
    RE::TESObjectWEAP* attackSource = RE::TESForm::LookupByID<RE::TESObjectWEAP>(event->source);

    if (attackSource->GetWeaponType() == RE::WEAPON_TYPE::kBow ||
        attackSource->GetWeaponType() == RE::WEAPON_TYPE::kCrossbow) {
        RE::DebugNotification("I can't destroy this lock with a bow");
    } else if (event->target->IsLocked() && event->cause->GetFormID() == 0x14) {
        RE::REFR_LOCK* targetLock = event->target->GetLock();
        if (targetLock) {
            logger::info("Target is locked");
            std::string_view formListId = GetFormList(event->target);
            HitThatLock(event->target->As<RE::TESObjectREFR>(), attackSource, formListId);
        } else {
            logger::trace("Target has no lock");
        }
    } else {
        logger::trace("Target is not an object");
    }

    return RE::BSEventNotifyControl::kContinue;
}

void BruteForce::UnlockObject(RE::TESObjectREFR* refr, bool IsTwoHanded, RE::TESObjectWEAP* weapon) {
    logger::info("Unlocking object");
    RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
    float factorsCombined = CalculateFactors(refr, player, weapon, IsTwoHanded ? RE::ActorValue::kTwoHanded : RE::ActorValue::kOneHanded);

    if (factorsCombined < 0.0) {
        refr->GetLock()->SetLocked(false);
        RE::PlaySound("NPCHumanWoodChopSD");

        IncreaseSkillExperience(IsTwoHanded ? RE::ActorValue::kTwoHanded : RE::ActorValue::kOneHanded, refr->GetLockLevel(), player);

        if (refr->GetActorOwner() != player->GetActorBase()) {
            player->StealAlarm(refr, refr->GetBaseObject(), 0, 0, refr->GetActorOwner(), false);
            logger::info("This is not owned by the player!!");
        } else {
            logger::info("This is owned by the player");
        }
    } else {
        RE::DebugNotification("This lock is too difficult to break open");
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

        logger::info("skillReq {}", skillReq);
        logger::info("skillLevel {}", skillLevel);
        logger::info("calculates {}", skillLevel >= skillReq);
        logger::info("onlyBlunt {}", onlyBlunt);
        logger::info("only2Handed {}", onlyTwoHanded);
        logger::info("is skill a req {}", IsSkillARequirement);

        if (onlyTwoHanded && onlyBlunt) {
            UnlockWithBluntAndTwoHanded(refr, weapon, skillLevel >= skillReq, IsSkillARequirement);
        } else if (onlyTwoHanded) {
            UnlockWithTwoHandedOnly(refr, weapon, skillLevel >= skillReq, IsSkillARequirement);
        } else if (onlyBlunt) {
            UnlockWithBluntOnly(refr, weapon, skillLevel >= skillReq, IsSkillARequirement, IsTwoHanded);
        } else {
            UnlockBasedOnMaterial(refr, IsTwoHanded, IsSkillARequirement, skillLevel >= skillReq, weapon);
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
            UnlockObject(refr, true, weapon);
        } else if (IsWeaponTwoHanded && !PlayerSkillMatches) {
            RE::DebugNotification("I'm not strong enough to break this lock");
        } else if (!IsWeaponTwoHanded) {
            RE::DebugNotification("Only a two handed weapon could break this lock");
        } else {
            RE::DebugNotification("This lock won't budge");
        }
    } else {
        if (IsWeaponTwoHanded) {
            UnlockObject(refr, true, weapon);
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
            UnlockObject(refr, IsWeaponTwoHanded, weapon);
        } else if (IsBluntWeapon && !PlayerSkillMatches) {
            RE::DebugNotification("I'm not strong enough to break this lock");
        } else if (!IsBluntWeapon) {
            RE::DebugNotification("Only a blunt weapon could break this lock");
        } else {
            RE::DebugNotification("This lock won't budge");
        }
    } else {
        if (IsBluntWeapon) {
            UnlockObject(refr, IsWeaponTwoHanded, weapon);
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
            UnlockObject(refr, true, weapon);
        } else if (IsWeaponWarhammer && !PlayerSkillMatches) {
            RE::DebugNotification("I'm not strong enough to break this lock");
        } else if (!IsWeaponWarhammer) {
            RE::DebugNotification("Only a warhammer could break this lock");
        } else {
            RE::DebugNotification("This lock won't budge");
        }
    } else {
        if (IsWeaponWarhammer) {
            UnlockObject(refr, true, weapon);
        } else {
            RE::DebugNotification("Only a warhammer could break this lock");
        }
    }
}

void BruteForce::UnlockBasedOnMaterial(RE::TESObjectREFR* refr, bool IsWeaponTwoHanded, bool IsUsingSkillRequirement,
                                       bool PlayerSkillMatches, RE::TESObjectWEAP* weapon) {
    if (IsUsingSkillRequirement && PlayerSkillMatches || !IsUsingSkillRequirement) {
        UnlockObject(refr, IsWeaponTwoHanded, weapon);
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

float BruteForce::NormalizeFactor(float a_level, float a_min, float a_max) { return ((a_level - a_max) / (a_max - a_min)) + 1.0f; }

float BruteForce::CalculateFactors(RE::TESObjectREFR* refr, RE::PlayerCharacter* Player, RE::TESObjectWEAP* weapon,
                                   RE::ActorValue skill) {
    Settings* Settings = Settings::GetSingleton();
    float fWeaponForce = 0.0;

    // TODO: Replace this if with something more performant
    if (weapon->HasKeywordString("WeapMaterialSteel")) {
        fWeaponForce = Settings->GetForceMultiplier("fSteel");
    } else if (weapon->HasKeywordString("WeapMaterialImperial")) {
        fWeaponForce = Settings->GetForceMultiplier("fImperial");
    } else if (weapon->HasKeywordString("WeapMaterialSilver")) {
        fWeaponForce = Settings->GetForceMultiplier("fSilver");
    } else if (weapon->HasKeywordString("WeapMaterialDwarven")) {
        fWeaponForce = Settings->GetForceMultiplier("fDwarven");
    } else if (weapon->HasKeywordString("WeapMaterialElven")) {
        fWeaponForce = Settings->GetForceMultiplier("fElven");
    } else if (weapon->HasKeywordString("WeapMaterialOrcish")) {
        fWeaponForce = Settings->GetForceMultiplier("fOrcish");
    } else if (weapon->HasKeywordString("DLC2WeaponMaterialNordic")) {
        fWeaponForce = Settings->GetForceMultiplier("´fNordic");
    } else if (weapon->HasKeywordString("WeapMaterialEbony")) {
        fWeaponForce = Settings->GetForceMultiplier("fEbony");
    } else if (weapon->HasKeywordString("DLC2WeaponMaterialStalhrim")) {
        fWeaponForce = Settings->GetForceMultiplier("fStalhrim");
    } else if (weapon->HasKeywordString("WeapMaterialDaedric")) {
        fWeaponForce = Settings->GetForceMultiplier("fDaedric");
    } else if (weapon->HasKeywordString("DLC1WeapMaterialDragonbone")) {
        fWeaponForce = Settings->GetForceMultiplier("fDragonbone");
    } else {
        fWeaponForce = 0.0;
    }

    float LockDiff = static_cast<float>(refr->GetLockLevel()) / 100;
    logger::info("Lock diff: {}", LockDiff);
    float LuckFactor = 0.95f + static_cast<float>(rand()) * static_cast<float>(1.05f - 0.95f) / RAND_MAX;
    logger::info("Luck factor: {}", LuckFactor);
    float PlayerForce = NormalizeFactor(static_cast<float>(Player->GetLevel()), 1.0, 81.0);
    logger::info("Player force: {}", PlayerForce);
    float WeaponSkill = NormalizeFactor(Player->GetActorBase()->GetActorValue(skill), 15.0, 100.0);
    logger::info("Weapon skill: {}", WeaponSkill);

    float ForceFactor = (WeaponSkill * 0.35f) + (fWeaponForce * 0.2f) + (PlayerForce * 0.45f);

    int SuccessFactor = static_cast<int>((((PlayerForce + LuckFactor) / 2) * 50 > rand() % 100 + 1));
    float Result = LockDiff - (ForceFactor * LuckFactor * SuccessFactor);
    logger::info("Result {}", Result);

    return Result;
}
