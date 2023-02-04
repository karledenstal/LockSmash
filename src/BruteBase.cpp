#include "BruteBase.h"
#include "Settings.h"

BruteBase* BruteBase::GetSingleton() {
    static BruteBase singleton;
    return &singleton;
}

bool BruteBase::IsTargetLocked(const RE::TESHitEvent* event) {
    return event->target->IsLocked() && event->cause->GetFormID() == 0x14;
}

float BruteBase::GetSkillRequirement(RE::LOCK_LEVEL lockLevel) {
    switch (lockLevel) {
        case RE::LOCK_LEVEL::kEasy:
            return Settings::GetSingleton()->skills.fApprenticeSkill;
        case RE::LOCK_LEVEL::kAverage:
            return Settings::GetSingleton()->skills.fAdeptSkill;
        case RE::LOCK_LEVEL::kHard:
            return Settings::GetSingleton()->skills.fExpertSkill;
        case RE::LOCK_LEVEL::kVeryHard:
            return Settings::GetSingleton()->skills.fMasterSkill;
        case RE::LOCK_LEVEL::kRequiresKey:
            return 900.0f;
        default:
            return Settings::GetSingleton()->skills.fNoviceSkill;
    }
}

std::string_view BruteBase::GetFormList(RE::LOCK_LEVEL lockLevel) {
    switch (lockLevel) {
        case RE::LOCK_LEVEL::kEasy:
            return "_BF_EasyLockMaterials";
        case RE::LOCK_LEVEL::kAverage:
            return "_BF_AverageLockMaterials";
        case RE::LOCK_LEVEL::kHard:
            return "_BF_HardLockMaterials";
        case RE::LOCK_LEVEL::kVeryHard:
        case RE::LOCK_LEVEL::kRequiresKey:
            return "_BF_VeryHardLockMaterials";
        default:
            return "_BF_VeryEasyLockMaterials";
    }
}

void BruteBase::UnlockedTarget(RE::TESObjectREFR* refr, RE::PlayerCharacter* player) { 
    logger::info("Unlocking object");
    
    if (refr && player) {
        refr->GetLock()->SetLocked(false);
        RE::PlaySound("NPCHumanWoodChopSD");
    } else {
        logger::info("Failed to instantiate player or refr");
    }
}

void BruteBase::CreateDetection(RE::TESObjectREFR* refr, RE::PlayerCharacter* player) {
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
}