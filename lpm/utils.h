#pragma once
#include <string>
#include <map>

namespace LPM::Utils {
    std::string format(
        std::string format_str,
        std::map<std::string, std::string> args
    );
}