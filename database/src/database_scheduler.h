#pragma once

#include <chrono>
#include <iostream>

#include "database_configdata.h"
#include "config_handler/config_handler.h"
#include "scheduler/scheduler.h"

struct DatabaseContext {
    DatabaseConfig config;
    ConfigHandler<DatabaseConfig> configHandler;
    nlohmann::json schemaJson;

    int checkIntervalSec = 30;
    int minModifiedAgeSec = 30;

    DatabaseContext(const std::string& configPath)
        : configHandler(configPath,
                        std::chrono::seconds(30),
                        std::chrono::seconds(30))
    {}
};

class DatabaseScheduler : public Scheduler<DatabaseContext> {
public:
    using Scheduler<DatabaseContext>::Scheduler;
    void processSecond(DatabaseContext& ctx) override;
};
