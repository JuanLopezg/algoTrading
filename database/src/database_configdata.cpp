#include "database_configdata.h"

// Validación simple del schema
static bool validateSchema(const nlohmann::json& j, const nlohmann::json& schema, std::string& err) {
    if (!j.contains("main_exchange")) {
        err = "Falta el campo obligatorio 'main_exchange'.";
        return false;
    }

    if (!j["main_exchange"].is_string()) {
        err = "'main_exchange' debe ser string.";
        return false;
    }

    if (j["main_exchange"].get<std::string>().empty()) {
        err = "'main_exchange' no puede estar vacío.";
        return false;
    }

    return true;
}

bool DatabaseConfig::loadFromJsonObject(const nlohmann::json& j,
                                        const nlohmann::json& schema,
                                        std::string& err)
{
    if (!validateSchema(j, schema, err))
        return false;

    main_exchange = j["main_exchange"].get<std::string>();
    return true;
}

bool DatabaseConfig::loadFromJsonFile(const std::string& configPath,
                                      const nlohmann::json& schema,
                                      std::string& err)
{
    std::ifstream fc(configPath);
    if (!fc.is_open()) {
        err = "No se pudo abrir config: " + configPath;
        return false;
    }

    nlohmann::json jconfig;
    fc >> jconfig;

    return loadFromJsonObject(jconfig, schema, err);
}
