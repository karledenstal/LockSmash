#pragma once
#include <stdint.h>

class Settings {
    public:
        [[nodiscard]] static Settings* GetSingleton();
    
        bool IsBruteForceEnabled();
        bool OnlyAllowBlunt();
        bool OnlyAllowTwoHanded();
        bool IsSkillRequirementEnabled();
        
        bool IsMagicEnabled();
        bool AllowShockToUnlock();
        
        double GetLockSkillReq(RE::LOCK_LEVEL lockLevel);
        float GetForceMultiplier(const char* a_key);
        float GetSkillIncreaseAmount(const char* level);
    
        struct BruteForceBasic {        
            bool bEnabled{true};
            bool bOnlyBlunt{false};
            bool bOnlyTwoHanded{false};
            bool bEnableSkillRequirement{true};
        } bruteForceBasic;

        struct Magic {
            bool bEnableMagic{true};
            bool bAllowShockToUnlock{false};
        } magic;
    
        struct Skills {
            double fNoviceSkill{0.0};
            double fApprenticeSkill{25.0};
            double fAdeptSkill{50.0};
            double fExpertSkill{75.0};
            double fMasterSkill{100.0};
            float fNoviceSkillIncrease{15.0f};
            float fApprenticeSkillIncrease{25.0f};
            float fAdeptSkillIncrease{35.0f};
            float fExpertSkillIncrease{55.0f};
            float fMasterSkillIncrease{75.0f};
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
};