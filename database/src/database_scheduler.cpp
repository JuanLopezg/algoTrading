#include "database_scheduler.h"
#include <spdlog/spdlog.h>

void DatabaseScheduler::processSecond(DatabaseContext& ctx) {

    spdlog::info("===== DatabaseScheduler tick =====");
    spdlog::info("Config actual:\n{}", ctx.config.toJson().dump(4));

    DatabaseConfig newConfig;
    std::string err;

    bool changed = ctx.configHandler.checkForUpdates(
        ctx.schemaJson,
        newConfig,
        err
    );

    if (changed) {
        spdlog::info("Nueva config detectada y validada.");
    } else if (!err.empty()) {
        spdlog::error("Error al validar config nueva: {}", err);
    }

    spdlog::info("processSecond ejecutado.");

    if (changed) {
        ctx.config = newConfig;
        spdlog::info("Config aplicada:\n{}", ctx.config.toJson().dump(4));
    }
}
