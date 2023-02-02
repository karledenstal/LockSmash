#pragma once

#include <random>

class BruteForce : public RE::BSTEventSink<RE::TESHitEvent> {
    BruteForce() = default;
    BruteForce(const BruteForce&) = delete;
    BruteForce(BruteForce&&) = delete;
    BruteForce& operator=(const BruteForce&) = delete;
    BruteForce& operator=(BruteForce&&) = delete;

    public:
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
        float NormalizeFactor(float a_level, float a_min, float a_max);
        void UnlockObject(RE::TESObjectREFR* refr, bool IsTwoHanded, RE::TESObjectWEAP* weapon);
        void HitThatLock(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, std::string_view formList);
        std::string_view GetFormList(RE::TESObjectREFRPtr refr);
        void UnlockWithTwoHandedOnly(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, bool PlayerSkillMatches,
                                     bool IsUsingSkillRequirement);
        void UnlockWithBluntOnly(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, bool PlayerSkillMatches,
                                 bool IsUsingSkillRequirement, bool IsWeaponTwoHanded);
        void UnlockWithBluntAndTwoHanded(RE::TESObjectREFR* refr, RE::TESObjectWEAP* weapon, bool PlayerSkillMatches,
                                         bool IsUsingSkillRequirement);
        void UnlockBasedOnMaterial(RE::TESObjectREFR* refr, bool IsWeaponTwoHanded, bool IsUsingSkillRequirement,
                                   bool PlayerSkillMatches, RE::TESObjectWEAP* weapon);
        void IncreaseSkillExperience(RE::ActorValue SkillToIncrease, RE::LOCK_LEVEL lockLevel,
                                     RE::PlayerCharacter* player);
        float CalculateFactors(RE::TESObjectREFR* refr, RE::PlayerCharacter* Player, RE::TESObjectWEAP* weapon, RE::ActorValue skill);
};