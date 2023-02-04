#include "Settings.h"

BruteBase* BruteBase::GetSingleton() {
    static BruteBase singleton;
    return &singleton;
}

RE::BSEventNotifyControl BruteBase::ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*) {
    if (event && event->target && event->source && event->cause) {
        logger::info("BruteForce: All events initialized");

        if (GetSingleton()->IsTargetLocked(event)) {
            auto* attackSourceWeapon = RE::TESForm::LookupByID<RE::TESObjectWEAP>(event->source);
            GetSingleton()->DisplaySealedByFate(event->target->GetLockLevel());

            // if the player attacks with a weapon
            if (attackSourceWeapon) {
                logger::info("Attacking with weapon: {}", attackSourceWeapon->GetName());

                auto formListId = GetSingleton()->GetFormList(event->target->GetLockLevel());
                
                if (BruteForce::GetSingleton()->isCorrectMaterial(attackSourceWeapon, formListId)) {
                    bool allowOnlyBlunt = Settings::GetSingleton()->bruteForceBasic.onlyAllowBlunt();
                    bool allowOnlyTwoHanded = Settings::GetSingleton()->bruteForceBasic.onlyAllowTwoHanded();

                    bool isWeaponTwoHanded = BruteForce::GetSingleton()->hasCorrectWeaponType(
                        attackSourceWeapon, BruteForce::Unlock::WeaponType::kTwoHanded);
                    
                    float skillRequirement = GetSingleton()->GetSkillRequirement(event->target->GetLockLevel());
                    float playerSkill =
                        RE::PlayerCharacter::GetSingleton()->GetActorBase()->GetActorValue(isWeaponTwoHanded ? RE::ActorValue::kTwoHanded : RE::ActorValue::kOneHanded);

                    BruteForce::Unlock::Flag flag;
                    
                    if (allowOnlyBlunt && allowOnlyTwoHanded) {
                        // if only Warhammers can open locks
                        flag = BruteForce::GetSingleton()->canUnlockSpecialized(
                            attackSourceWeapon, playerSkill >= skillRequirement, 
                            BruteForce::Unlock::WeaponType::kWarhammer);
                    } else if (allowOnlyBlunt) {
                        // if only blunt weapons can open locks
                        flag = BruteForce::GetSingleton()->canUnlockSpecialized(
                            attackSourceWeapon, playerSkill >= skillRequirement,
                            BruteForce::Unlock::WeaponType::kBlunt);

                    } else if (allowOnlyTwoHanded) {
                        // if only two-handed weapons can open locks
                        flag = BruteForce::GetSingleton()->canUnlockSpecialized(
                            attackSourceWeapon, playerSkill >= skillRequirement,
                            BruteForce::Unlock::WeaponType::kTwoHanded);

                    } else {
                        // if all weapons can open locks
                        flag = BruteForce::GetSingleton()->canUnlockBasic(playerSkill >= skillRequirement);
                    }

                    if (flag == BruteForce::Unlock::Flag::kPasses) {
                        GetSingleton()->UnlockWithWeapon(event->target->As<RE::TESObjectREFR>(), attackSourceWeapon);
                    } else {
                        GetSingleton()->DisplayNoUnlock(flag);
                    }
                    
                } else {
                    RE::DebugNotification("This lock is too sturdy for this weapon");
                }

                return RE::BSEventNotifyControl::kContinue;
            }
        
            // if the player attacks with magic
            if (Settings::GetSingleton()->magic.isMagicEnabled()) {
                auto* attackSourceMagic = RE::TESForm::LookupByID<RE::SpellItem>(event->source);
                if (attackSourceMagic) {
                    logger::info("Attacking with magic: {}", attackSourceMagic->GetName());
                }
                return RE::BSEventNotifyControl::kContinue;
            }
        }
        
        return RE::BSEventNotifyControl::kContinue;
    }

    return RE::BSEventNotifyControl::kContinue;
}

void BruteBase::UnlockWithWeapon(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon) {
    RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
    bool isTwoHanded = BruteForce::GetSingleton()->hasCorrectWeaponType(weapon, BruteForce::Unlock::WeaponType::kTwoHanded);
    auto skillUsed = isTwoHanded ? RE::ActorValue::kTwoHanded : RE::ActorValue::kOneHanded;
    auto skillReq = GetSingleton()->GetSkillRequirement(refr->GetLockLevel());
    auto fChanceOfSuccess = BruteForce::GetSingleton()->GetSuccessChance(weapon, skillUsed, skillReq);
    
    if ((rand() % 100) < fChanceOfSuccess) {
        GetSingleton()->UnlockTarget(refr, player);
        BruteForce::GetSingleton()->IncreaseSkillExperience(skillUsed, refr->GetLockLevel(), player);
    } else {
        RE::DebugNotification("This lock is too difficult");
    }

    GetSingleton()->CreateDetection(refr, player);
}

void BruteBase::DisplayNoUnlock(BruteForce::Unlock::Flag flag) {
    switch (flag) { 
        case BruteForce::Unlock::Flag::kSkillFail :
            RE::DebugNotification("I'm not strong enough to break this lock");
            break;
        case BruteForce::Unlock::Flag::kWrongWeaponType:
            RE::DebugNotification("I need a weapon of another type");
            break;
        default:
            RE::DebugNotification("This lock won't budge");
            break;
    }
}

void BruteBase::DisplaySealedByFate(RE::LOCK_LEVEL lockLevel) {
    if (lockLevel == RE::LOCK_LEVEL::kRequiresKey) {
        RE::DebugNotification("This lock is sealed by Fate");
    }
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

void BruteBase::UnlockTarget(RE::TESObjectREFR* refr, RE::PlayerCharacter* player) { 
    logger::info("Unlocking object");
    
    if (refr && player) {
        refr->GetLock()->SetLocked(false);
        RE::PlaySound("NPCHumanWoodChopSD");
        RE::DebugNotification("The lock is broken");
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