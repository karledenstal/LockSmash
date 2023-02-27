class SmashOpen : public RE::BSTEventSink<RE::TESHitEvent> {
public:
    [[nodiscard]] static SmashOpen* GetSingleton();
    RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*);

private:
    struct LockMagicProps {
        bool lockIsFrosted = false;
        bool lockIsShocked = false;
        bool lockIsBurning = false;
    } lockMagicProps;

    enum class WeaponType {
        kBlunt,
        kTwoHanded,
        kWarhammer,  // if both blunt & two handed is checked
        kAll,
    };
        
    enum class MagicType {
        kShock,
        kFire,
        kFrost,
    };

    enum class Status {
        kSuccess,
        kFailed,
        kSkillFailed,
        kWrongType
    };

    void UnlockWithMagic(RE::TESObjectREFR* refr, RE::SpellItem* spell);
    bool MagicCanUnlock(RE::EffectSetting* effect);
    bool IsAllowedToUnlock(RE::SpellItem* spell);
    void UnlockIt(RE::REFR_LOCK* lock);
    float GetMagicSuccessChance(RE::SpellItem* spellUsed, RE::LOCK_LEVEL lockLevel);
    float GetSkillReq(RE::LOCK_LEVEL lockLevel);
};
