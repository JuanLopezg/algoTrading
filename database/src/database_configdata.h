#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <fstream>

struct DatabaseConfig {
    std::string main_exchange = "undefined";

    // Valida JSON contra schema
    bool loadFromJsonObject(const nlohmann::json& j, const nlohmann::json& schema, std::string& err);

    // Lee fichero config + valida con schema
    bool loadFromJsonFile(const std::string& configPath,
                          const nlohmann::json& schema,
                          std::string& err);

    // Para ConfigHandler (no usado realmente, pero requerido por la plantilla)
    bool loadFromStream(std::istream&) { return true; }

    // JSON actual de la config
    nlohmann::json toJson() const {
        return nlohmann::json{
            {"main_exchange", main_exchange}
        };
    }
};
