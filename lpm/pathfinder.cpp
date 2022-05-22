#include <vector>
#include <fstream>
#include "macros.h"
#include "pathfinder.h"
#include "utils.h"

std::string LPM::PathFinder::locate_config() {
    const std::vector<std::string> PATHS = {
        LPM_CONFIG_PATHS
    };

    for (auto raw_path : PATHS) {
        std::string path = raw_path;
        LPM::Env::fill_env_vars(path);

        std::ifstream file(path);
        if (file.good()) {
            return path;
        }
    }

    return "";
}

std::string LPM::PathFinder::locate_packages_toml() {
    // Locate the packages.toml file in the current and parent directories
    // up to LPM_FIND_MAX_DEPTH.
    std::string packages_toml_path = LPM_PACKAGES_MANIFEST_NAME;

    bool found = Utils::fs::exists(packages_toml_path);
    if (!found) {
        for (int i = 1; i < LPM_FIND_MAX_DEPTH; i++) {
            // Repeat ../ i times
            packages_toml_path = Utils::repeat("../", i) + LPM_PACKAGES_MANIFEST_NAME;
            found = Utils::fs::exists(packages_toml_path);
            if (found) {
                return packages_toml_path;
            }
        }
    } else {
        return packages_toml_path;
    }

    return "";
}