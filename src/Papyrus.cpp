#include "Papyrus.h"

#include "Settings.h"

namespace Papyrus {
    void SOSBF_MCM::OnConfigClose(RE::TESQuest*) { 
        Settings::GetSingleton()->LoadSettings(); 
    }

    bool SOSBF_MCM::Register(RE::BSScript::IVirtualMachine* a_vm) {
        a_vm->RegisterFunction("OnConfigClose", "SOSBF_MCM", OnConfigClose);

        logger::info("Registered SOSBF_MCM");
        return true;
    }

    void Register() {
        auto papyrus = SKSE::GetPapyrusInterface();
        papyrus->Register(SOSBF_MCM::Register);
        logger::info("Registered papyrus functions");
    }
}