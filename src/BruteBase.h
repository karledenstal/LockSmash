class BruteBase : public RE::BSTEventSink<RE::TESHitEvent>, public BruteForce {
	public:
        BruteBase() = default;
        BruteBase(const BruteBase&) = delete;
        BruteBase(BruteBase&&) = delete;
        void operator=(const BruteBase&) = delete;
        void operator=(BruteBase&&) = delete;
    
    [[nodiscard]] static BruteBase* GetSingleton();
    RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*);
    
    private:
        void UnlockTarget(RE::TESObjectREFR* refr, RE::PlayerCharacter* player);
        bool IsTargetLocked(const RE::TESHitEvent* event);
        std::string_view GetFormList(RE::LOCK_LEVEL lockLevel);
        void CreateDetection(RE::TESObjectREFR* refr, RE::PlayerCharacter* player);
        float GetSkillRequirement(RE::LOCK_LEVEL lockLevel);

        void DisplaySealedByFate(RE::LOCK_LEVEL lockLevel);
        
        void DisplayNoUnlock(BruteForce::Unlock::Flag flag);

        void UnlockWithWeapon(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon);
};
