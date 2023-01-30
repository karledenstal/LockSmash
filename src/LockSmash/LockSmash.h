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
        auto targetName = event->target->GetBaseObject()->GetName();
        auto sourceName = event->cause->GetBaseObject()->GetName();
        logger::info("{} hit {}", sourceName, targetName);
        if (event->flags.any(RE::TESHitEvent::Flag::kPowerAttack)) logger::info("Ooooo power attack!");
        return RE::BSEventNotifyControl::kContinue;
    }
};
