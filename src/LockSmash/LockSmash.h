#include "../../logger.h"

class LockSmash : public RE::BSTEventSink<RE::TESHitEvent> {
    LockSmash() = default;
    LockSmash(const LockSmash&) = delete;
    LockSmash(LockSmash&&) = delete;
    LockSmash& operator=(const LockSmash&) = delete;
    LockSmash& operator=(LockSmash&&) = delete;

public:
    static LockSmash* GetSingleton() {
        static LockSmash singleton;
        return &singleton;
    }

    RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*) {
        auto* attackSource = RE::TESForm::LookupByID<RE::TESObjectWEAP>(event->source);

        if (attackSource) {
            if (event->target->IsLocked() && event->cause->GetFormID() == 0x14) {
                auto* targetLock = event->target->GetLock();
                if (targetLock) {
                    logger::info("Target is locked");
                    TryToUnlock(attackSource, event->target);
                } else {
                    logger::trace("Target has no lock");
                }
            } else {
                logger::trace("Target is not an object");
            }
        }


        
        return RE::BSEventNotifyControl::kContinue;
    }

    private:
        inline static void TryToUnlock(RE::TESObjectWEAP* weapon, RE::TESObjectREFRPtr refr) {
            logger::info("ref: {}", refr->GetName());
            auto lockLevel = refr->GetLockLevel();

            std::string_view formListId = "_BF_VeryEasyLockMaterials";

            if (lockLevel == RE::LOCK_LEVEL::kEasy) {
                formListId = "_BF_EasyLockMaterials";
            } else if (lockLevel == RE::LOCK_LEVEL::kAverage) {
                formListId = "_BF_AverageLockMaterials";
            } else if (lockLevel == RE::LOCK_LEVEL::kHard) {
                formListId = "_BF_HardLockMaterials";
            } else if (lockLevel == RE::LOCK_LEVEL::kVeryHard) {
                formListId = "_BF_VeryHardLockMaterials";
            } else if (lockLevel == RE::LOCK_LEVEL::kRequiresKey) {
                RE::DebugNotification("This lock requires a key");
                return;
            }

            HitThatLock(refr->As<RE::TESObjectREFR>(), weapon, formListId);
        }

        inline static void HitThatLock(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, std::string_view formList) {
            if (weapon->HasKeywordInList(RE::TESForm::LookupByEditorID<RE::BGSListForm>(formList), false)) {
                logger::info("This weapon can break this lock");
                UnlockObject(refr);
            } else {
                RE::DebugNotification("This lock is too sturdy for this weapon");
            }
        }
        
        inline static void UnlockObject(RE::TESObjectREFR* refr) { 
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
        }
};
