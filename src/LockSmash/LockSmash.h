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
        auto lockLevel = event->target->GetLockLevel();
        //auto* player = RE::TESForm::LookupByID<RE::PlayerCharacter>(0x14);

        //auto* leftHandedEquip = player->GetEquippedObject(true);
        //auto* rightHandedEquip = player->GetEquippedObject(false); 

        //if (leftHandedEquip) {
        //    logger::info("Player left hand: {}", leftHandedEquip->GetName());
        //}

        //if (rightHandedEquip) {
        //    logger::info("Player right hand: {}", rightHandedEquip->GetName());
        //}

        switch (lockLevel) {
            case RE::LOCK_LEVEL::kRequiresKey: {
                logger::info("This lock requires a key");
                break;
            }
            case RE::LOCK_LEVEL::kVeryEasy: {
                logger::info("Novice lock");
                UnlockObject(event->target->As<RE::TESObjectREFR>());
                break;
            }
            case RE::LOCK_LEVEL::kEasy: {
                logger::info("Apprentice lock");
                break;
            }
            case RE::LOCK_LEVEL::kAverage: {
                logger::info("Adept lock");
                break;
            }
            case RE::LOCK_LEVEL::kHard: {
                logger::info("Expert lock");
                break;
            }
            case RE::LOCK_LEVEL::kVeryHard: {
                logger::info("Master lock");
                break;
            }
            default:
                logger::info("Not even locked, dude");
        }

        return RE::BSEventNotifyControl::kContinue;
    }

    private:
    inline static void UnlockObject(RE::TESObjectREFR* refr) { 
        logger::info("Unlocking object");
        refr->GetLock()->SetLocked(false);
    }
};
