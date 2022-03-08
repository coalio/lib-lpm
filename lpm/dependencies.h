#pragma once
#include <string>
#include <utility>
#include "manifests.h"

using namespace LPM::Manifests;

namespace LPM::Dependencies {
    typedef std::pair<const std::string, std::string> Dependency;

    bool is_installed(const Dependency& dependency);

    bool install(
        const Dependency& dependency,
        Repository::Package& package,
        std::string cache_path,
        std::string package_path,
        std::string& error
    );

    bool unpack(const std::string& package_url, const std::string& package_path);
}