#include "Papyrus.h"

#include "Settings.h"

namespace Papyrus {
    void SmashOpen_MCM::OnConfigClose(RE::TESQuest*) { 
        Settings::GetSingleton()->LoadSettings(); 
    }

    bool SmashOpen_MCM::Register(RE::BSScript::IVirtualMachine* a_vm) {
        a_vm->RegisterFunction("OnConfigClose", "SmashOpen_MCM", OnConfigClose);

        logger::info("Registered SmashOpen_MCM");
        return true;
    }

    void Register() {
        auto papyrus = SKSE::GetPapyrusInterface();
        papyrus->Register(SmashOpen_MCM::Register);
        logger::info("Registered papyrus functions");
    }
}