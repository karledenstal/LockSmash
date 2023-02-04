#pragma once

class BruteMagic : public RE::BSTEventSink<RE::TESHitEvent> {
    BruteMagic() = default;
    BruteMagic(const BruteMagic&) = delete;
    BruteMagic(BruteMagic&&) = delete;
    BruteMagic& operator=(const BruteMagic&) = delete;
    BruteMagic& operator=(BruteMagic&&) = delete;

public:
    [[nodiscard]] static BruteMagic* GetSingleton();
    RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*);

    
};
