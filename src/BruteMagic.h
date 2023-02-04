#pragma once

#include "Settings.h"

class BruteMagic : public RE::BSTEventSink<RE::TESHitEvent> {
    public:
        BruteMagic() = default;
        BruteMagic(const BruteMagic&) = delete;
        BruteMagic(BruteMagic&&) = delete;
        void operator=(const BruteMagic&) = delete;
        void operator=(BruteMagic&&) = delete;
        
        [[nodiscard]] static BruteMagic* GetSingleton();
        RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*);
};
