#pragma once
#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class Logger {
public:
    static void init(const std::string& name = "default") {
        auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        logger = std::make_shared<spdlog::logger>(name, sink);
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        logger->set_level(spdlog::level::info);

        spdlog::set_default_logger(logger);
    }

    static std::shared_ptr<spdlog::logger>& get() {
        return logger;
    }

    template<typename... Args>
    static void info(fmt::format_string<Args...> fmt, Args&&... args) {
        logger->info(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void warn(fmt::format_string<Args...> fmt, Args&&... args) {
        logger->warn(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void error(fmt::format_string<Args...> fmt, Args&&... args) {
        logger->error(fmt, std::forward<Args>(args)...);
    }

private:
    inline static std::shared_ptr<spdlog::logger> logger = nullptr;
};
