#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

#include <boost/program_options.hpp>

#include "logger/logger.h"
#include "database_scheduler.h"

namespace po = boost::program_options;

int main(int argc, char** argv) {
    // Inicializar logger global
    Logger::init("database");
    Logger::info("Iniciando Database...");

    // Variables CLI
    std::string configPath;
    std::string schemaPath;
    int checkInterval = 30;
    int minModifiedAge = 30;

    // -------------------------------
    //   Parseo de argumentos
    // -------------------------------
    try {
        po::options_description desc("Opciones");
        desc.add_options()
            ("help,h", "Mostrar ayuda")
            ("config,c", po::value<std::string>(&configPath)->required(),
                "Ruta al archivo de configuración JSON")
            ("schema,s", po::value<std::string>(&schemaPath)->required(),
                "Ruta al archivo JSON schema")
            ("check-interval,i", po::value<int>(&checkInterval)->default_value(30),
                "Cada cuántos segundos verificar si la config ha cambiado")
            ("min-modified-age,m", po::value<int>(&minModifiedAge)->default_value(30),
                "Edad mínima (en segundos) desde la última modificación para recargar");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        po::notify(vm);
    } 
    catch (const std::exception& e) {
        Logger::error("Error en argumentos: {}", e.what());
        return 1;
    }

    Logger::info(
        "Config={}, Schema={}, CheckInterval={}, MinAge={}",
        configPath, schemaPath, checkInterval, minModifiedAge
    );

    // -------------------------------
    //   Crear contexto
    // -------------------------------
    DatabaseContext ctx(configPath);
    ctx.checkIntervalSec = checkInterval;
    ctx.minModifiedAgeSec = minModifiedAge;

    // Reconfigurar configHandler
    ctx.configHandler = ConfigHandler<DatabaseConfig>(
        configPath,
        std::chrono::seconds(checkInterval),
        std::chrono::seconds(minModifiedAge)
    );

    // -------------------------------
    //   Leer schema
    // -------------------------------
    {
        std::ifstream fs(schemaPath);
        if (!fs.is_open()) {
            Logger::error("No se pudo abrir schema en '{}'", schemaPath);
            return 1;
        }
        fs >> ctx.schemaJson;
    }

    // -------------------------------
    //   Cargar config inicial
    // -------------------------------
    {
        std::string err;
        if (!ctx.config.loadFromJsonFile(configPath, ctx.schemaJson, err)) {
            Logger::error("Error en config inicial: {}", err);
            return 1;
        }
    }

    Logger::info("Config inicial:\n{}", ctx.config.toJson().dump(4));

    // -------------------------------
    //   Iniciar scheduler
    // -------------------------------
    DatabaseScheduler scheduler(
        ctx,
        std::chrono::seconds(1),   // cada cuanto corre processSecond
        std::chrono::seconds(60)   // timeout
    );

    scheduler.start();

    Logger::info("Ejecutando... CTRL+C para salir");

    // -------------------------------
    //   Loop infinito
    // -------------------------------
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    scheduler.stop();
    return 0;
}
