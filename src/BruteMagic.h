#pragma once

#include "Settings.h"

class BruteMagic {
    public:
        BruteMagic() = default;
        BruteMagic(const BruteMagic&) = delete;
        BruteMagic(BruteMagic&&) = delete;
        void operator=(const BruteMagic&) = delete;
        void operator=(BruteMagic&&) = delete;
        
        [[nodiscard]] static BruteMagic* GetSingleton();

     private:
        void UnlockObject();
        float GetSuccessChance(RE::SpellItem* spell, float fSkillReq);
};
