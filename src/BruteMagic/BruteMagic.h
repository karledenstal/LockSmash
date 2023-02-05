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

        bool getIsAllowedType(RE::MagicSystem::SpellType a_type);
        float GetSuccessChance(RE::SpellItem* spell, float fSkillReq);
        bool isAllowedMagic(RE::SpellItem* spell);
        void IncreaseMagicSkill(RE::ActorValue skillUsed, RE::PlayerCharacter* player, RE::LOCK_LEVEL lockLevel);
        Unlock::Flag canUnlock(bool skillCheckPasses, RE::MagicSystem::CastingType castingType);
};
