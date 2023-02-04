#pragma once

#include "../Settings.h"

class BruteMagic {
    public:
        BruteMagic() = default;
        BruteMagic(const BruteMagic&) = delete;
        BruteMagic(BruteMagic&&) = delete;
        void operator=(const BruteMagic&) = delete;
        void operator=(BruteMagic&&) = delete;
        
        [[nodiscard]] static BruteMagic* GetSingleton();

        struct Unlock {
            enum class Flag {
                kSkillFailed,
                kWrongCastingType,
                kFail,
                kPasses,
            };
        };

        void UnlockObject();
        float GetSuccessChance(RE::SpellItem* spell, float fSkillReq);
        bool isAllowedMagic(RE::SpellItem* spell);
        void IncreaseMagicSkill(RE::PlayerCharacter* player, RE::LOCK_LEVEL lockLevel);
        Unlock::Flag canUnlock(bool skillCheckPasses, RE::MagicSystem::CastingType castingType);
};
