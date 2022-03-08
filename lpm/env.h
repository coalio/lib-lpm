#pragma once
#include <cstdlib>
#include <string>

namespace LPM::Env {
    std::string get(std::string key, std::string default_value );
    int get(std::string key, int default_value);

    void fill_env_vars(std::string& path, bool replace_empty = true);
}