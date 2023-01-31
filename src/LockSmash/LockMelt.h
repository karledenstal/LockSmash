#include "../../logger.h"

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
        return RE::BSEventNotifyControl::kContinue;
    };
};