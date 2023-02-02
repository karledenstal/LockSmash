#include "src/LockSmash.h"
#include "src/Settings.h"

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
            logger::info("BruteForce: Data loaded");
            break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);

    // Setup logging (e.g. using spdlog)
    SetupLog();
    
    SKSE::GetMessagingInterface()->RegisterListener(OnInit);

    try {
        if (Settings::GetSingleton()->isBruteForceEnabled()) {
            logger::info("BruteForce: Enabled");
            auto* eventSource = RE::ScriptEventSourceHolder::GetSingleton();
            eventSource->AddEventSink<RE::TESHitEvent>(LockSmash::GetSingleton());
        } else {
            logger::info("BruteForce: Disabled");
        }
    } catch (...) {
        logger::error("Exception caught when loading settings! Default settings will be used");
    };

    return true;
}
