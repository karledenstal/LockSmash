#pragma once
#include "Settings.h"

class BruteForce : public RE::BSTEventSink<RE::TESHitEvent>, public BruteBase {
    public:
        BruteForce() = default;
        BruteForce(const BruteForce&) = delete;
        BruteForce(BruteForce&&) = delete;
        void operator=(const BruteForce&) = delete;
        void operator=(BruteForce&&) = delete;
        [[nodiscard]] static BruteForce* GetSingleton();
        RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* event, RE::BSTEventSource<RE::TESHitEvent>*);
        
        enum WEAP_MATERIAL {
            kIron,
            kSteel,
            kSilver,
            kImperial,
            kElven,
            kDwarven,
            kOrcish,
            kNordic,
            kEbony,
            kStalhrim,
            kGlass,
            kDaedric,
            kDragonbone,
        };

    private:
        void UnlockObject(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, bool IsTwoHanded);
        void HitThatLock(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, std::string_view formList);
        void UnlockWithTwoHandedOnly(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, bool PlayerSkillMatches,
                                     bool IsUsingSkillRequirement);
        void UnlockWithBluntOnly(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, bool PlayerSkillMatches,
                                 bool IsUsingSkillRequirement, bool IsWeaponTwoHanded);
        void UnlockWithBluntAndTwoHanded(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, bool PlayerSkillMatches,
                                         bool IsUsingSkillRequirement);
        void UnlockBasedOnMaterial(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, bool IsWeaponTwoHanded,
                                   bool IsUsingSkillRequirement,
                                   bool PlayerSkillMatches);
        void IncreaseSkillExperience(RE::ActorValue SkillToIncrease, RE::LOCK_LEVEL lockLevel,
                                     RE::PlayerCharacter* player);
        float GetSuccessChance(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, RE::ActorValue SkillUsed);
        float GetWeaponMultiplier(RE::TESObjectWEAP* weapon);
};