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
    
        struct Basic {  
            void Load(CSimpleIniA& a_ini);

            bool isEnabled();
            bool onlyAllowBlunt();
            bool onlyAllowTwoHanded();
            bool isSkillRequirementEnabled();
            bool isWeaponTypeRequirementEnabled();
            
            bool bEnabled{true};
            bool bOnlyBlunt{false};
            bool bOnlyTwoHanded{false};
            bool bEnableSkillRequirement{true};
            bool bEnableWeaponTypeRequirement{true};
        } basic;

        struct Magic {
            void Load(CSimpleIniA& a_ini);

            bool isMagicEnabled();
            bool allowShockToUnlock();
            bool allowAlteration();
            bool onlyFireAndForget();
            bool isConcentratedDamageDebuffEnabled();
            
            float getFrostLockBuff();
            float getConcentratedDebuff();
            
            bool bEnableMagic{true};
            bool bAllowShockToUnlock{false};
            bool bOnlyFireAndForget{false};
            bool bConcentratedDamageDebuff{true};
            bool bAllowAlteration{true};
            
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

        // thx PO3
        private:
            struct detail {
                template <class T>
                static void config(CSimpleIniA& a_ini, T& a_value, const char* a_section, const char* a_key) {
                    if constexpr (std::is_same_v<bool, T>) {
                        a_value = a_ini.GetBoolValue(a_section, a_key, a_value);
                        a_ini.SetBoolValue(a_section, a_key, a_value);
                        logger::info("Setting: {} with {} in section {}", a_key, a_value, a_section);
                    } else if constexpr (std::is_floating_point_v<T>) {
                        a_value = static_cast<T>(a_ini.GetDoubleValue(a_section, a_key, a_value));
                        a_ini.SetDoubleValue(a_section, a_key, a_value);
                        logger::info("Setting: {} with {} in section {}", a_key, std::to_string(a_value).c_str(), a_section);
                    } else if constexpr (std::is_arithmetic_v<T> || std::is_enum_v<T>) {
                        a_value = std::string::lexical_cast<T>(a_ini.GetValue(a_section, a_key, std::to_string(a_value).c_str()));
                        a_ini.SetValue(a_section, a_key, std::to_string(a_value).c_str());
                        logger::info("Setting: {} with {} in section {}", a_key, std::to_string(a_value).c_str(), a_section);
                    } else {
                        a_value = a_ini.GetValue(a_section, a_key, a_value.c_str());
                        a_ini.SetValue(a_section, a_key, a_value.c_str());
                        logger::info("Setting: {} with {} in section {}", a_key, a_value, a_section);
                    }
                }
            };
};