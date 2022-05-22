#pragma once
#include <string>
#include "lpm/env.h"

namespace LPM::PathFinder {
    std::string locate_config();
    std::string locate_packages_toml();
}