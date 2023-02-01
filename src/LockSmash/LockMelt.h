#include "../../logger.h"

// This is future feature hopefully
// Melt locks using alchemy

class LockMelt : public RE::BSTEventSink<RE::TESActivateEvent> {
    LockMelt() = default;
    LockMelt(const LockMelt&) = delete;
    LockMelt(LockMelt&&) = delete;
    LockMelt& operator=(const LockMelt&) = delete;
    LockMelt& operator=(LockMelt&&) = delete;

public:
    static LockMelt* GetSingleton() {
        static LockMelt singleton;
        return &singleton;
    }

    RE::BSEventNotifyControl ProcessEvent(const RE::TESActivateEvent* event,
                                          RE::BSTEventSource<RE::TESActivateEvent>*) {
        logger::info("Activate event", event->objectActivated->GetName());
        return RE::BSEventNotifyControl::kContinue;
    };
};