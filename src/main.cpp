#include <kinetica/rendering.hpp>
#include <kinetica/log.hpp>
#include <kinetica/types.hpp>
#include <kinetica/window.hpp>
#include <kinetica/ecs/registry.hpp>

#include <kinetica/ecs/components/transform.hpp>
#include <kinetica/ecs/components/material.hpp>
#include <kinetica/ecs/components/mesh.hpp>

#include <iostream>

Kinetica::SAppArgs parse_args(int argc, char* argv[]) {
    Kinetica::SAppArgs args;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            args.showHelp = true;
        } else if (arg == "--version" || arg == "-v") {
            args.showVersion = true;
        } else if (arg == "--headless") {
            args.headless = true;
        } else if (arg.starts_with("--log-level=")) {
            args.logLevel = arg.substr(13);
        } else if (arg.starts_with("--plugin-dir=")) {
            args.pluginDir = arg.substr(14);
        } else if (arg.starts_with("--")) {
            KLOG_ERROR("Unknown option: " + arg);
        } else {
            // Assume it's a file to open
            args.filesToOpen.push_back(arg);
        }
    }

    return args;
}

void print_help() {
    std::cout << R"(Kinetica - Low-poly 3D modeling, reimagined
Usage: kinetica [options] [file.kin ...]

Options:
  -h, --help          Show this help message
  -v, --version       Show version info
      --headless      Run without UI (for batch processing)
      --log-level=L   Set log level (debug, info, warn, error)
      --plugin-dir=P  Load plugins from directory P
)";
}

void print_version() {
    std::cout << R"(Kinetica - Low-poly 3D modeling, reimagined
version : 0.0.1,
publisher : Initialized
)";
}


int main(int argc, char* argv[]) {
    Kinetica::SAppArgs args = parse_args(argc, argv);
    if (args.showHelp) {
        print_help();
        return static_cast<int>(Kinetica::EExitCode::Success);
    }

    if (args.showVersion) {
        print_version();
        return static_cast<int>(Kinetica::EExitCode::Success);
    }

    if (args.headless) {
        KLOG_INFO("Running in headless mode (not implemented yet)");
        return static_cast<int>(Kinetica::EExitCode::Success);
    }

    Kinetica::CWindow window;
    if (!window.isValid()) {
        return static_cast<int>(Kinetica::EExitCode::InitializationFailed);
    }

    glewInit();
    Kinetica::CRenderer renderer(window);

    Kinetica::CRegistry registry;

    while (!window.shouldClose()) {
        window.pollEvents();

        if (window.isMinimized()) { window.swap(); continue; }

        renderer.clear();

        const auto allEntities = registry.getAllEntities();
        for (const auto& entity : allEntities) {
            auto* transform = registry.getComponent<Kinetica::Components::STransform>(entity);
            auto* mesh      = registry.getComponent<Kinetica::Components::SMesh>(entity);
            auto* material  = registry.getComponent<Kinetica::Components::SMaterial>(entity);

            if (transform && mesh && material) {
                // renderer.renderEntity(*transform, *mesh, *material);
            }
        }

        window.swap();
    }

    return static_cast<int>(Kinetica::EExitCode::Success);
}
