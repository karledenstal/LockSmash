#include "BruteForce.h"
#include "Settings.h"

BruteForce* BruteForce::GetSingleton() {
    static BruteForce singleton;
    return &singleton;
}

RE::BSEventNotifyControl BruteForce::ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*) {
    auto* attackSource = RE::TESForm::LookupByID<RE::TESObjectWEAP>(event->source);

    if (event->target->IsLocked() && event->cause->GetFormID() == 0x14) {
        auto* targetLock = event->target->GetLock();
        if (targetLock) {
            logger::info("Target is locked");
            auto formListId = GetFormList(event->target);
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

    auto* player = RE::PlayerCharacter::GetSingleton();

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
        auto* settings = Settings::GetSingleton();
        bool onlyBlunt = settings->OnlyAllowBlunt();
        bool onlyTwoHanded = settings->OnlyAllowTwoHanded();
        bool isSkillARequirement = settings->IsSkillRequirementEnabled();

        if (isSkillARequirement) {
            auto lockLevel = refr->GetLockLevel();
            auto* player = RE::PlayerCharacter::GetSingleton()->GetActorBase();
            float skillReq = settings->GetLockSkillReq(lockLevel);
            bool IsTwoHanded = weapon->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>("_BF_TwoHandedTypes"), false);
            bool IsBlunt = weapon->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>("_BF_BluntWeapons"), false);
            auto skillLevel = player->GetActorValue(IsTwoHanded ? RE::ActorValue::kTwoHanded : RE::ActorValue::kOneHanded);
            
            bool PlayerSkillIsBetterOrEqual = skillLevel >= skillReq;
            
            logger::info("skill level: {}", skillLevel);
            logger::info("skill req: {}", skillReq);

            if (onlyTwoHanded) {
                if (IsTwoHanded && PlayerSkillIsBetterOrEqual) {
                    UnlockObject(refr);
                    return;
                } else if (IsTwoHanded && !PlayerSkillIsBetterOrEqual) {
                    RE::DebugNotification("I'm not strong enough to break this lock");
                } else if (!IsTwoHanded && PlayerSkillIsBetterOrEqual) {
                    RE::DebugNotification("Only a two handed weapon could break this lock");
                } else {
                    RE::DebugNotification("This lock won't budge");
                }
            }
            
            if (skillLevel >= skillReq && !onlyTwoHanded && !onlyBlunt) {
                UnlockObject(refr);
                return;
            } else {
                RE::DebugNotification("I'm not strong enough to break this lock");
            }
        }
        
        logger::info("This weapon can break this lock");
        UnlockObject(refr);
    } else {
        RE::DebugNotification("This lock is too sturdy for this weapon");
    }
}

std::string_view BruteForce::GetFormList(RE::TESObjectREFRPtr refr) {
    auto lockLevel = refr->GetLockLevel();

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
