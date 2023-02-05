#pragma once

class Settings {
    public:
        Settings() = default;
        Settings(const Settings&) = delete;
        Settings(Settings&&) = delete;
        void operator=(const Settings&) = delete;
        void operator=(Settings&&) = delete;
    
        [[nodiscard]] static Settings* GetSingleton();

        void LoadSettings();
    
        struct BruteForceBasic {  
            void Load(CSimpleIniA& a_ini);

            bool isEnabled();
            bool onlyAllowBlunt();
            bool onlyAllowTwoHanded();
            bool isSkillRequirementEnabled();
            
            bool bEnabled{true};
            bool bOnlyBlunt{false};
            bool bOnlyTwoHanded{false};
            bool bEnableSkillRequirement{true};
        } bruteForceBasic;

        struct Magic {
            void Load(CSimpleIniA& a_ini);

            bool isMagicEnabled();
            bool allowShockToUnlock();
            bool onlyFireAndForget();
            bool isConcentratedDamageDebuffEnabled();
            
            float getFrostLockBuff();
            float getConcentratedDebuff();
            
            bool bEnableMagic{true};
            bool bAllowShockToUnlock{false};
            bool bOnlyFireAndForget{false};
            bool bConcentratedDamageDebuff{true};
            
            float fConcentratedDamageDebuff{10.0f};
            float fFrostedLockBuff{8.0f};
        } magic;

        struct SuccessChance {
            void Load(CSimpleIniA& a_ini);
            
            float getMaxChance();
            float getMinChance();
            
            float fMaxChance{95.0f};
            float fMinChance{5.0f};
        } successChance;
    
        struct Skills {
            void Load(CSimpleIniA& a_ini);
            
            float fNoviceSkill{0.0f};
            float fApprenticeSkill{25.0f};
            float fAdeptSkill{50.0f};
            float fExpertSkill{75.0f};
            float fMasterSkill{100.0f};
            float fNoviceSkillIncrease{15.0f};
            float fApprenticeSkillIncrease{25.0f};
            float fAdeptSkillIncrease{35.0f};
            float fExpertSkillIncrease{55.0f};
            float fMasterSkillIncrease{75.0f};
        } skills;

        struct Multipliers {
            void Load(CSimpleIniA& a_ini);
            
            float fIron{0.0f};
            float fSteel{15.0f};
            float fSilver{13.0f};
            float fImperial{16.0f};
            float fElven{19.0f};
            float fDwarven{21.0f};
            float fOrcish{30.0f};
            float fNordic{28.0f};
            float fEbony{37.0f};
            float fStalhrim{35.0f};
            float fGlass{26.5f};
            float fDaedric{40.0f};
            float fDragonbone{45.0f};
        } multipliers;

        // thx PO3
        private:
            struct detail {
                template <class T>
                static void config(CSimpleIniA& a_ini, T& a_value, const char* a_section, const char* a_key) {
                    if constexpr (std::is_same_v<bool, T>) {
                        a_value = a_ini.GetBoolValue(a_section, a_key, a_value);
                        a_ini.SetBoolValue(a_section, a_key, a_value);
                    } else if constexpr (std::is_floating_point_v<T>) {
                        a_value = static_cast<T>(a_ini.GetDoubleValue(a_section, a_key, a_value));
                        a_ini.SetDoubleValue(a_section, a_key, a_value);
                    } else if constexpr (std::is_arithmetic_v<T> || std::is_enum_v<T>) {
                        a_value = std::string::lexical_cast<T>(a_ini.GetValue(a_section, a_key, std::to_string(a_value).c_str()));
                        a_ini.SetValue(a_section, a_key, std::to_string(a_value).c_str());
                    } else {
                        a_value = a_ini.GetValue(a_section, a_key, a_value.c_str());
                        a_ini.SetValue(a_section, a_key, a_value.c_str());
                    }
                }
            };
};