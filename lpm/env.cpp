#include "env.h"
#include "macros.h"

std::string LPM::Env::get(std::string key, std::string default_value ) {
    const char* value = std::getenv(key.c_str());
    if (value == NULL) {
        return default_value;
    }

    return value;
}

int LPM::Env::get(std::string key, int default_value) {
    const char* value = std::getenv(key.c_str());

    if (value == NULL) {
        return default_value;
    }

    return std::atoi(value);
}

void LPM::Env::fill_env_vars(std::string& path, bool replace_empty) {
    // Replace ${key} with the value of the environment variable named key
    size_t start_pos = path.find("${");
    while (start_pos != std::string::npos) {
        size_t end_pos = path.find("}", start_pos);
        if (end_pos == std::string::npos) {
            break;
        }

        std::string key = path.substr(start_pos + 2, end_pos - start_pos - 2);
        std::string value = get(key, "");
        if (value == "" and replace_empty || value != "") {
            path.replace(start_pos, end_pos - start_pos + 1, value);
        }

        start_pos = path.find("${", start_pos + value.length() + 1);
    }
}