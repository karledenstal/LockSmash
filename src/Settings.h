#pragma once
#include <stdint.h>
#include "BruteForce.h"

class Settings {
    public:
        [[nodiscard]] static Settings* GetSingleton();
    
        void Load();

        bool IsBruteForceEnabled();
        bool OnlyAllowBlunt();
        bool OnlyAllowTwoHanded();
        bool IsSkillRequirementEnabled();
        
        double GetLockSkillReq(RE::LOCK_LEVEL lockLevel);
        float GetForceMultiplier(BruteForce::WEAP_MATERIAL material);
    
        struct BruteForceBasic {        
            bool bEnabled{true};
            bool bOnlyBlunt{false};
            bool bOnlyTwoHanded{false};
            bool bEnableSkillRequirement{true};
        } bruteForceBasic;
    
        struct Skills {
            double fNoviceSkill{0.0};
            double fApprenticeSkill{25.0};
            double fAdeptSkill{50.0};
            double fExpertSkill{75.0};
            double fMasterSkill{100.0};
        } skills;

        struct Multipliers {
            float fIron{0.0f};
            float fSteel{0.2f};
            float fSilver{0.2f};
            float fImperial{0.3f};
            float fElven{0.4f};
            float fDwarven{0.4f};
            float fOrcish{0.6f};
            float fNordic{0.5f};
            float fEbony{0.7f};
            float fStalhrim{0.8f};
            float fGlass{0.7f};
            float fDaedric{1.0f};
            float fDragonbone{1.0f};
        } multipliers;

    private: 
        struct detail {
            template <class T>
            static void get_value(CSimpleIniA& a_ini, T& a_value, const char* a_section, const char* a_key, const char* a_comment) {
                if constexpr (std::is_same_v<bool, T>) {
                    a_value = a_ini.GetBoolValue(a_section, a_key, a_value);
                    a_ini.SetBoolValue(a_section, a_key, a_value, a_comment);
                } else if constexpr (std::is_floating_point_v<T>) {
                    a_value = static_cast<T>(a_ini.GetDoubleValue(a_section, a_key, a_value));
                    a_ini.SetDoubleValue(a_section, a_key, a_value, a_comment);
                } else {
                    a_value = a_ini.GetValue(a_section, a_key, a_value.c_str());
                    a_ini.SetValue(a_section, a_key, a_value.c_str(), a_comment);
                }
            };
        };
};