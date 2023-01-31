#include "logger.h"
#include "src/LockSmash/LockSmash.h"
#include "src/LockSmash/LockMelt.h"

void OnInit(SKSE::MessagingInterface::Message* msg) { 
    switch (msg->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            logger::info("BruteForce: Data loaded");
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
    eventSource->AddEventSink<RE::TESActivateEvent>(LockMelt::GetSingleton());

    return true;
}
