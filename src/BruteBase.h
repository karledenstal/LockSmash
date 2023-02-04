class BruteBase {
	public:
        BruteBase() = default;
        BruteBase(const BruteBase&) = delete;
        BruteBase(BruteBase&&) = delete;
        void operator=(const BruteBase&) = delete;
        void operator=(BruteBase&&) = delete;
    
    [[nodiscard]] static BruteBase* GetSingleton();
    
    void UnlockedTarget(RE::TESObjectREFR* refr, RE::PlayerCharacter* player);
    bool IsTargetLocked(const RE::TESHitEvent* event);
    std::string_view GetFormList(RE::LOCK_LEVEL lockLevel);
    void CreateDetection(RE::TESObjectREFR* refr, RE::PlayerCharacter* player);
    float GetSkillRequirement(RE::LOCK_LEVEL lockLevel);
};
