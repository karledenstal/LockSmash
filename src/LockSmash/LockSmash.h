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
        auto* targetObject = event->target->As<RE::TESObjectREFR>();
        auto causeOfAction = event->cause->GetFormID();
        bool isPlayerResponsible = causeOfAction == 0x14;

        if (targetObject && isPlayerResponsible) {
            auto* targetLock = targetObject->GetLock();
            if (targetLock) {
                if (targetLock->IsLocked()) {
                    logger::info("Target is locked");
                    TryToUnlock(attackSource, targetObject);
                } else {
                    logger::trace("Target is not locked");
                }
            } else {
                logger::trace("Target has no lock");
                return RE::BSEventNotifyControl::kStop;
            }
        } else {
            logger::trace("Target is not an object");
            return RE::BSEventNotifyControl::kStop;
        }
        
        return RE::BSEventNotifyControl::kContinue;
    }

    private:
        inline static void TryToUnlock(RE::TESObjectWEAP* weapon, RE::TESObjectREFR* refr) {
            logger::info("ref: {}", refr->GetName());
            auto lockLevel = refr->GetLockLevel();

            if (weapon->HasKeywordString("WeapTypeBow")) {
                RE::DebugNotification("Can't unlock with bow");
                return;
            }

            std::string_view formListId;

            if (lockLevel == RE::LOCK_LEVEL::kVeryEasy) {
                formListId = "_BF_VeryEasyLockMaterials";
            } else if (lockLevel == RE::LOCK_LEVEL::kEasy) {
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

            HitThatLock(refr, weapon, formListId);
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
        }
};
