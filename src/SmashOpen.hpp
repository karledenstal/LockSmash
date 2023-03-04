class SmashOpen : public RE::BSTEventSink<RE::TESHitEvent> {
public:
    [[nodiscard]] static SmashOpen* GetSingleton();
    RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*);

private:
    struct LockMagicProps {
        enum class LockProp {
            kFrosted,
            kShocked,
            kBurning,
        };
        
        bool getLockProps(LockProp prop);
        
        bool lockIsFrosted = false;
        bool lockIsShocked = false;
        bool lockIsBurning = false;
    } lockMagicProps;

    bool Initialize(const RE::TESHitEvent* event);

    RE::ActorValue GetAssociatedSkill(RE::TESObjectWEAP* weapon);
    bool MagicCanUnlock(RE::EffectSetting* effect);
    bool IsSpellAllowedToUnlock(RE::SpellItem* spell);
    void UnlockIt(RE::TESObjectREFRPtr target);
    float GetSkillReq(RE::LOCK_LEVEL lockLevel);
    void Unlock(RE::TESObjectREFRPtr target, RE::FormID source);
    float CalculateWeaponSuccessChance(RE::TESObjectWEAP* weaponUsed, RE::LOCK_LEVEL lockLevel);
    float CalculateSpellSuccessChance(RE::SpellItem* spellUsed, RE::LOCK_LEVEL lockLevel);
    float GetSkillIncrease(RE::LOCK_LEVEL lockLevel);
    void CreateDetectionEvent();
};
