#include "Settings.h"
#include "BruteBase.h"

BruteBase* BruteBase::GetSingleton() {
    static BruteBase singleton;
    return &singleton;
}

RE::BSEventNotifyControl BruteBase::ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*) {
    if (event && event->target && event->source && event->cause && Settings::GetSingleton()->basic.isEnabled()) {
        logger::info("Smash Open SKSE: Enabled");
        logger::info("BruteForce: All events initialized");

        if (GetSingleton()->IsTargetLocked(event)) {
            auto* attackSourceWeapon = RE::TESForm::LookupByID<RE::TESObjectWEAP>(event->source);
            GetSingleton()->DisplaySealedByFate(event->target->GetLockLevel());

            // if the player attacks with a weapon
            if (attackSourceWeapon) {
                logger::info("Attacking with weapon: {}", attackSourceWeapon->GetName());

                auto formListId = GetSingleton()->GetFormList(event->target->GetLockLevel());
                
                if (BruteForce::GetSingleton()->isCorrectMaterial(attackSourceWeapon, formListId)) {
                    bool allowOnlyBlunt = Settings::GetSingleton()->basic.onlyAllowBlunt();
                    bool allowOnlyTwoHanded = Settings::GetSingleton()->basic.onlyAllowTwoHanded();

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
                        GetSingleton()->DisplayNoWeaponUnlock(flag);
                    }
                    
                } else {
                    RE::DebugNotification("This lock is too sturdy for this weapon");
                }
            }
        
            // if the player attacks with magic
            if (Settings::GetSingleton()->magic.isMagicEnabled()) {
                logger::info("Smash Open SKSE Magic: Enabled");
                auto* attackSourceMagic = RE::TESForm::LookupByID<RE::SpellItem>(event->source);
                if (attackSourceMagic) {
                    logger::info("Attacking with magic: {}", attackSourceMagic->GetName());

                    if (BruteMagic::GetSingleton()->isAllowedMagic(attackSourceMagic)) {
                        float skillRequirement = GetSingleton()->GetSkillRequirement(event->target->GetLockLevel());
                        float playerSkill = RE::PlayerCharacter::GetSingleton()->GetActorBase()->GetActorValue(RE::ActorValue::kDestruction);
                        
                        BruteMagic::Unlock::Flag flag = BruteMagic::GetSingleton()->canUnlock(playerSkill >= skillRequirement, attackSourceMagic->GetCastingType());

                        if (flag == BruteMagic::Unlock::Flag::kPasses) {
                            GetSingleton()->UnlockWithMagic(event->target->As<RE::TESObjectREFR>(), attackSourceMagic);
                        } else {
                            GetSingleton()->DisplayNoMagicUnlock(flag);
                        }
                    } else {
                        RE::DebugNotification("This magic isn't sufficient");
                    }
                }
            }
        }
    }

    return RE::BSEventNotifyControl::kContinue;
}

void BruteBase::UnlockWithWeapon(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon) {
    RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
    bool isTwoHanded = BruteForce::GetSingleton()->hasCorrectWeaponType(weapon, BruteForce::Unlock::WeaponType::kTwoHanded);
    auto skillUsed = isTwoHanded ? RE::ActorValue::kTwoHanded : RE::ActorValue::kOneHanded;
    auto skillReq = GetSingleton()->GetSkillRequirement(refr->GetLockLevel());

    auto fChanceOfSuccess = BruteForce::GetSingleton()->GetSuccessChance(weapon, skillUsed, skillReq, BruteMagic::GetSingleton()->lockMagicProps.getLockIsFrosted());
    
    if ((rand() % 101) < fChanceOfSuccess || !Settings::GetSingleton()->basic.isSkillRequirementEnabled()) {
        GetSingleton()->UnlockTarget(refr, player);
        BruteForce::GetSingleton()->IncreaseSkillExperience(skillUsed, refr->GetLockLevel(), player);
    } else {
        RE::DebugNotification("This lock is too difficult");
    }

    GetSingleton()->CreateDetection(refr, player);
}

void BruteBase::UnlockWithMagic(RE::TESObjectREFR* refr, RE::SpellItem* spell) {
    RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
    auto fChanceOfSuccess = BruteMagic::GetSingleton()->GetSuccessChance(spell, GetSingleton()->GetSkillRequirement(refr->GetLockLevel()));
        
    if ((rand() % 101) < fChanceOfSuccess || !Settings::GetSingleton()->basic.isSkillRequirementEnabled()) {
        GetSingleton()->UnlockTarget(refr, player);
        BruteMagic::GetSingleton()->IncreaseMagicSkill(spell->GetAssociatedSkill(), player, refr->GetLockLevel());
    } else {
        RE::DebugNotification("This lock is too difficult");
    }

    GetSingleton()->CreateDetection(refr, player);
}

void BruteBase::DisplayNoMagicUnlock(BruteMagic::Unlock::Flag flag) {
    switch (flag) {
        case BruteMagic::Unlock::Flag::kSkillFailed:
            RE::DebugNotification("I'm not skilled enough to break this lock");
            break;
        case BruteMagic::Unlock::Flag::kWrongCastingType:
            RE::DebugNotification("I need a different type of magic for this lock");
            break;
        default:
            RE::DebugNotification("This lock won't budge");
    }
}

void BruteBase::DisplayNoWeaponUnlock(BruteForce::Unlock::Flag flag) {
    switch (flag) { 
        case BruteForce::Unlock::Flag::kSkillFail :
            RE::DebugNotification("I'm not strong enough to break this lock");
            break;
        case BruteForce::Unlock::Flag::kWrongWeaponType:
            RE::DebugNotification("I need a weapon of another type");
            break;
        default:
            RE::DebugNotification("This lock won't budge");
    }
}

void BruteBase::DisplaySealedByFate(RE::LOCK_LEVEL lockLevel) {
    if (lockLevel == RE::LOCK_LEVEL::kRequiresKey) {
        RE::DebugNotification("This is sealed by Fate");
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
        player->TrespassAlarm(refr, refr->GetActorOwner(), 1000);        
    } else {
        logger::info("This is owned by the player");
    }
}