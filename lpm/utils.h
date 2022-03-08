#pragma once
#include <zip.h>
#include <string>
#include <map>
#include <vector>
#include <filesystem>

namespace LPM::Utils {
    namespace fs = std::filesystem;

    void format(
        std::string& format_str,
        std::map<std::string, std::string> args
    );

    bool write_file(
        const std::string& path,
        const std::string& content
    );

    bool unzip(
        zip* zip_file,
        const std::string& dest_path,
        std::string& error
    );

    std::vector<std::string> split(
        const std::string& str,
        char delimiter
    );

    std::string join(
        std::vector<std::string>& segments,
        char delimiter,
        size_t start_index = 0,
        size_t end_index = std::string::npos
    );
}