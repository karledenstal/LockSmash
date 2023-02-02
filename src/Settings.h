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
        double GetForceMultiplier(BruteForce::WEAP_MATERIAL material);
    
        struct BruteForceBasic {
            void Load(CSimpleIniA& a_ini);
        
            bool bEnabled{true};
            bool bOnlyBlunt{false};
            bool bOnlyTwoHanded{false};
            bool bEnableSkillRequirement{true};
        } bruteForceBasic;
    
        struct Skills {
            void Load(CSimpleIniA& a_ini);

            double iNoviceSkill{0.0};
            double iApprenticeSkill{25.0};
            double iAdeptSkill{50.0};
            double iExpertSkill{75.0};
            double iMasterSkill{100.0};
        } skills;

        struct Multipliers {
            void Load(CSimpleIniA& a_ini);

            double fIron{0.0};
            double fSteel{0.2};
            double fSilver{0.2};
            double fImperial{0.3};
            double fElven{0.4};
            double fDwarven{0.4};
            double fOrcish{0.6};
            double fNordic{0.5};
            double fEbony{0.75};
            double fStalhrim{0.8};
            double fGlass{0.7};
            double fDaedric{1.0};
            double fDragonbone{1.0};
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