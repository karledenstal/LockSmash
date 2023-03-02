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

    bool Initialize(const RE::TESHitEvent* event);

    RE::ActorValue GetAssociatedSkill(RE::TESObjectWEAP* weapon);
    bool MagicCanUnlock(RE::EffectSetting* effect);
    bool IsSpellAllowedToUnlock(RE::SpellItem* spell);
    void UnlockIt(RE::TESObjectREFRPtr target, RE::ActorValue skillUsed);
    float GetSkillReq(RE::LOCK_LEVEL lockLevel);
    void Unlock(RE::TESObjectREFRPtr target, RE::FormID source);
    float CalculateWeaponSuccessChance(RE::TESObjectWEAP* weaponUsed, RE::LOCK_LEVEL lockLevel);
    float CalculateSpellSuccessChance(RE::SpellItem* spellUsed, RE::LOCK_LEVEL lockLevel);
    float GetSkillIncrease(RE::LOCK_LEVEL lockLevel);
};
