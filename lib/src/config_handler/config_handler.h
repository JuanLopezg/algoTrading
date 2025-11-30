#pragma once

#include <string>
#include <filesystem>
#include <chrono>
#include <mutex>
#include <optional>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

// ---------------------------------------------------------------------
// IMPORTANTE: el handler debe usar siempre system_clock.
// steady_clock NO sirve para comparar timepoints de filesystem.
// ---------------------------------------------------------------------

template<typename TConfig>
class ConfigHandler {
public:
    ConfigHandler() = default;

    ConfigHandler(
        const std::string& configPath,
        std::chrono::seconds checkInterval,
        std::chrono::seconds minModifiedAge
    )
        : path(configPath),
          reloadInterval(checkInterval),
          minModifiedAge(minModifiedAge),
          lastCheck(std::chrono::system_clock::now())
    {}

    bool checkForUpdates(const nlohmann::json& schema,
                         TConfig& outConfig,
                         std::string& err)
    {
        using namespace std::chrono;

        auto now = system_clock::now();

        // Esperar checkInterval antes de volver a chequear
        if (now - lastCheck < reloadInterval)
            return false;

        lastCheck = now;

        // Obtener tiempo de modificación del fichero
        auto ftime = std::filesystem::last_write_time(path);

        // Convertir file_clock → system_clock
        auto sctp = time_point_cast<system_clock::duration>(
            ftime - decltype(ftime)::clock::now() + system_clock::now()
        );

        // Ahora sí: comparar tiempos (mismo reloj)
        auto age = now - sctp;

        if (age < minModifiedAge)
            return false;

        // Intentar cargar nueva config validada
        if (!outConfig.loadFromJsonFile(path, schema, err))
            return false;

        return true;
    }

private:
    std::string path;

    std::chrono::seconds reloadInterval {30};
    std::chrono::seconds minModifiedAge {30};

    std::chrono::time_point<std::chrono::system_clock> lastCheck;
};
