#include "Settings.h"
#include "Papyrus.h"
#include "SmashOpen.hpp"

void SetupLog() {
    auto logsFolder = SKSE::log::log_directory();
    if (!logsFolder) SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
    auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
    auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
    spdlog::set_default_logger(std::move(loggerPtr));
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::trace);
}

void OnInit(SKSE::MessagingInterface::Message* msg) { 
    switch (msg->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            try {
                Settings::GetSingleton()->LoadSettings();
                auto* eventSource = RE::ScriptEventSourceHolder::GetSingleton();
                eventSource->AddEventSink<RE::TESHitEvent>(SmashOpen::GetSingleton());
            } catch (...) {
                logger::error("Exception caught when loading settings! Default settings will be used");
            };
            break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);

    // Setup logging (e.g. using spdlog)
    SetupLog();
    
    auto messaging = SKSE::GetMessagingInterface();
    
    if (!messaging->RegisterListener("SKSE", OnInit)) {
        return false;
    }
    
    messaging->RegisterListener(OnInit);
    Papyrus::Register();

    return true;
}
