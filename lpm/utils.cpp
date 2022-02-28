#include "utils.h"

std::string LPM::Utils::format(
    std::string format_str,
    std::map<std::string, std::string> args
) {
    // Replace ${key} with the value of the environment variable named key
    size_t start_pos = format_str.find("${");
    while (start_pos != std::string::npos) {
        size_t end_pos = format_str.find("}", start_pos);
        if (end_pos == std::string::npos) {
            break;
        }

        std::string key = format_str.substr(
            start_pos + 2,
            end_pos - start_pos - 2
        );

        std::string value = args[key];

        if (value == "") {
            throw std::runtime_error(
                "Failed to format string: " + format_str +
                " because key \"" + key + "\" is empty"
            );
        }

        format_str.replace(
            start_pos,
            end_pos - start_pos + 1,
            value
        );

        start_pos = format_str.find("${", start_pos + value.length());
    }

    return format_str;
}