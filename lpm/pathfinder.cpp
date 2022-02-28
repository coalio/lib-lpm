#include <vector>
#include <fstream>
#include "macros.h"
#include "pathfinder.h"

std::string LPM::PathFinder::locate_config() {
    const std::vector<std::string> PATHS = {
        LPM_CONFIG_PATHS
    };

    for (auto raw_path : PATHS) {
        std::string path = LPM::Env::fill_env_vars(raw_path);
        LPM_PRINT_DEBUG("Looking in path: " << path);
        std::ifstream file(path);

        if (file.good()) {
            return path;
        }
    }

    return "";
}