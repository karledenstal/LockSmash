#pragma once
#include "../Settings.h"

class BruteForce {
    public:
        BruteForce() = default;
        BruteForce(const BruteForce&) = delete;
        BruteForce(BruteForce&&) = delete;
        void operator=(const BruteForce&) = delete;
        void operator=(BruteForce&&) = delete;
        [[nodiscard]] static BruteForce* GetSingleton();
        
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

        struct Unlock {
            enum class Flag {
                kPasses,
                kSkillFail,
                kWrongWeaponType,
                kFail,
            };

            enum class WeaponType {
                kBlunt,
                kTwoHanded,
                kWarhammer,  // if both blunt & two handed is checked
                kAll, 
            };
        };
        
       

    public:
        void IncreaseSkillExperience(RE::ActorValue SkillToIncrease, RE::LOCK_LEVEL lockLevel,
                                     RE::PlayerCharacter* player);
        float GetSuccessChance(RE::TESObjectWEAP* weapon, RE::ActorValue SkillUsed, float fSkillReq, bool lockIsFrosted);
        float GetWeaponMultiplier(RE::TESObjectWEAP* weapon);
        Unlock::Flag canUnlockSpecialized(RE::TESObjectWEAP* weapon, bool skillCheckPasses,
                                         Unlock::WeaponType weaponType);
        Unlock::Flag canUnlockBasic(bool skillCheckPasses);
        bool isCorrectMaterial(RE::TESObjectWEAP* weapon, std::string_view formList);
        bool hasCorrectWeaponType(RE::TESObjectWEAP* weapon, Unlock::WeaponType weaponType);
};