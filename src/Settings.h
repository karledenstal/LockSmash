#pragma once
#include <stdint.h>

class Settings {
    public:
        [[nodiscard]] static Settings* GetSingleton();
    
        void Load();

        bool isBruteForceEnabled();
    
        struct BruteForce {
            void Load(CSimpleIniA& a_ini);
        
            bool bEnabled{true};
            bool bOnlyBlunt{false};
            bool bOnlyTwoHanded{false};
            bool bEnableSkillRequirement{true};
        } bruteForce;
    
        struct Skills {
            void Load(CSimpleIniA& a_ini);

            std::uint32_t iNoviceSkill{0};
            std::uint32_t iApprenticeSkill{25};
            std::uint32_t iAdeptSkill{50};
            std::uint32_t iExpertSkill{75};
            std::uint32_t iMasterSkill{100};
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
            double fStahlrim{0.8};
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
                }
            };
        };
};