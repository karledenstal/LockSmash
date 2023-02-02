#include "BruteForce.h"
#include "Settings.h"

BruteForce* BruteForce::GetSingleton() {
    static BruteForce singleton;
    return &singleton;
}

RE::BSEventNotifyControl BruteForce::ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*) {
    RE::TESObjectWEAP* attackSource = RE::TESForm::LookupByID<RE::TESObjectWEAP>(event->source);

    if (event->target->IsLocked() && event->cause->GetFormID() == 0x14) {
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

void BruteForce::UnlockObject(RE::TESObjectREFR* refr) {
    logger::info("Unlocking object");
    refr->GetLock()->SetLocked(false);

    RE::PlaySound("NPCHumanWoodChopSD");

    RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

    if (refr->GetActorOwner() != player->GetActorBase()) {
        // steal alarm?
        // not sure how this works at all
        player->StealAlarm(refr, refr->GetBaseObject(), 0, 0, refr->GetActorOwner(), false);
        logger::info("This is not owned by the player!!");
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

        float skillReq = settings->GetLockSkillReq(lockLevel);
        float skillLevel = player->GetActorValue(IsTwoHanded ? RE::ActorValue::kTwoHanded : RE::ActorValue::kOneHanded);

        logger::info("Skill level: {}", skillLevel);
        logger::info("Skill req: {}", skillReq);

         if (onlyTwoHanded) {
            UnlockWithTwoHandedOnly(refr, weapon, skillLevel >= skillReq, IsSkillARequirement);
        } else if (onlyBlunt) {
            UnlockWithBluntOnly(refr, weapon, skillLevel >= skillReq, IsSkillARequirement);
        } else if (onlyTwoHanded && onlyBlunt) {
            UnlockWithBluntAndTwoHanded(refr, weapon, skillLevel >= skillReq, IsSkillARequirement);
        } else {
            UnlockBasedOnMaterial(refr, IsSkillARequirement, skillLevel >= skillReq);
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
            UnlockObject(refr);
        } else if (IsWeaponTwoHanded && !PlayerSkillMatches) {
            RE::DebugNotification("I'm not strong enough to break this lock");
        } else if (!IsWeaponTwoHanded && PlayerSkillMatches) {
            RE::DebugNotification("Only a two handed weapon could break this lock");
        } else {
            RE::DebugNotification("This lock won't budge");
        }
    } else {
        if (IsWeaponTwoHanded) {
            UnlockObject(refr);
        } else {
            RE::DebugNotification("Only a two handed weapon could break this lock");
        }
    }
}

void BruteForce::UnlockWithBluntOnly(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, bool PlayerSkillMatches,
                                     bool IsUsingSkillRequirement) {
    bool IsBluntWeapon = weapon->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>("_BF_BluntWeapons"), false);

    if (IsUsingSkillRequirement) {
        if (IsBluntWeapon && PlayerSkillMatches) {
            UnlockObject(refr);
        } else if (IsBluntWeapon && !PlayerSkillMatches) {
            RE::DebugNotification("I'm not strong enough to break this lock");
        } else if (!IsBluntWeapon && PlayerSkillMatches) {
            RE::DebugNotification("Only a blunt weapon could break this lock");
        } else {
            RE::DebugNotification("This lock won't budge");
        }
    } else {
        if (IsBluntWeapon) {
            UnlockObject(refr);
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
            UnlockObject(refr);
        } else if (IsWeaponWarhammer && !PlayerSkillMatches) {
            RE::DebugNotification("I'm not strong enough to break this lock");
        } else if (!IsWeaponWarhammer && PlayerSkillMatches) {
            RE::DebugNotification("Only a warhammer could break this lock");
        } else {
            RE::DebugNotification("This lock won't budge");
        }
    } else {
        if (IsWeaponWarhammer) {
            UnlockObject(refr);
        } else {
            RE::DebugNotification("Only a warhammer could break this lock");
        }
    }
}

void BruteForce::UnlockBasedOnMaterial(RE::TESObjectREFR* refr, bool IsUsingSkillRequirement, bool PlayerSkillMatches) {
    if (IsUsingSkillRequirement && PlayerSkillMatches || !IsUsingSkillRequirement) {
        UnlockObject(refr);
    } else {
        RE::DebugNotification("I'm not strong enough to break this lock");
    }
}
