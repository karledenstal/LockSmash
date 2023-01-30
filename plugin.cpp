#include "logger.h"
#include "src/mini/ini.h"
#include "src/LockSmash/LockSmash.h"

void OnInit(SKSE::MessagingInterface::Message* msg) { 
    switch (msg->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            logger::info("Data loaded");
            break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);

    // Setup logging (e.g. using spdlog)
    SetupLog();
    
    SKSE::GetMessagingInterface()->RegisterListener(OnInit);

    auto* eventSource = RE::ScriptEventSourceHolder::GetSingleton();
    eventSource->AddEventSink<RE::TESHitEvent>(LockSmash::GetSingleton());

    return true;
}
