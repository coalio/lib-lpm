#include "manifests.h"
#include "macros.h"
#include "toml11/toml.hpp"

void LPM::Manifests::Packages::load() {
    std::ifstream file(this->path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open package manifest file: " + this->path);
    }

    toml::value data = toml::parse(this->path);
    this->name = toml::find_or(data, "project", "name", "");
    this->version = toml::find_or(data, "project", "version", "");
    this->description = toml::find_or(data, "project", "description", "");
    this->author = toml::find_or(data, "project", "author", "");
    this->license = toml::find_or(data, "project", "license", "");
    this->homepage = toml::find_or(data, "project", "homepage", "");
    this->repository = toml::find_or(data, "project", "repository", "");
    this->main = toml::find_or(data, "project", "main", "");
    this->lua_version = toml::find_or(data, "project", "lua_version", "");

    if (data.contains("dependencies")) {
        this->dependencies = toml::find<
            std::map<std::string, std::string>
        >(data, "dependencies");
    }

    LPM_PRINT_DEBUG("Loaded data for project: " << this->name);

    // print all of the parsed data
    LPM_PRINT_DEBUG("this->name : " << this->name);
    LPM_PRINT_DEBUG("this->version : " << this->version);
    LPM_PRINT_DEBUG("this->description : " << this->description);
    LPM_PRINT_DEBUG("this->author : " << this->author);
    LPM_PRINT_DEBUG("this->license : " << this->license);
    LPM_PRINT_DEBUG("this->homepage : " << this->homepage);
    LPM_PRINT_DEBUG("this->repository : " << this->repository);
    LPM_PRINT_DEBUG("this->main : " << this->main);
    LPM_PRINT_DEBUG("this->lua_version : " << this->lua_version);
    LPM_PRINT_DEBUG("this->dependencies.size() : " << this->dependencies.size());
    for (auto& dependency : this->dependencies) {
        LPM_PRINT_DEBUG("this->dependencies : " << dependency.first << " " << dependency.second);
    }
}

void LPM::Manifests::Packages::save() {
    std::ofstream file(this->path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open packages file: " + this->path);
    }

    toml::value data;

    data["project"] = toml::value {
        {"name", this->name},
        {"version", this->version},
        {"description", this->description},
        {"author", this->author},
        {"license", this->license},
        {"homepage", this->homepage},
        {"repository", this->repository},
        {"main", this->main},
        {"lua_version", this->lua_version}
    };

    if (this->dependencies.size() > 0) {
        data["dependencies"] = this->dependencies;
    }

    try {
        file << data;
    } catch (...) {
        throw std::runtime_error("Failed to write to file: " + this->path);
    }
}

void LPM::Manifests::Config::load() {
    std::ifstream file(this->path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + this->path);
    }

    toml::value data = toml::parse(this->path);
    this->db_backend = toml::find_or(data, "lpm", "db_backend", "");
    this->packages_db = toml::find_or(data, "lpm", "packages_db", "");
    this->repositories_cache = toml::find_or(data, "lpm", "repositories_cache", "");
    this->packages_cache = toml::find_or(data, "lpm", "packages_cache", "");
    this->modules_path = toml::find_or(data, "lpm", "modules_path", "");

    std::string missing_keys = "";

    if (this->db_backend == "") {
        missing_keys += " db_backend";
    }

    if (this->packages_db == "") {
        missing_keys += " packages_db";
    }

    if (this->repositories_cache == "") {
        missing_keys += " repositories_cache";
    }

    if (this->packages_cache == "") {
        missing_keys += " packages_cache";
    }

    if (this->modules_path == "") {
        missing_keys += " modules_path";
    }

    if (missing_keys != "") {
        throw std::runtime_error(
            "Your 'lpm' section in your lpm.toml configuration file is missing the following parameters:" + missing_keys
        );
    }

    if (data.contains("luas")) {
        this->luas = toml::find<
            std::map<std::string, std::string>
        >(data, "luas");

        if (!this->luas.contains("default")) {
            throw std::runtime_error("You must specify a 'default' Lua interpreter in your lpm.toml file, under 'luas' section.");
        }
    } else {
        throw std::runtime_error("No Lua interpreters specified in the lpm.toml file. Specify them using the 'luas' section");
    }

    if (data.contains("sources")) {
        this->repositories = toml::find<
            std::map<
                std::string,
                std::map<std::string, std::string>
            >
        >(data, "sources");
    } else {
        throw std::runtime_error("No repositories specified in the lpm.toml file.");
    }

    LPM_PRINT_DEBUG("Loaded data for config: " << this->path);

    // print all of the parsed data
    LPM_PRINT_DEBUG("this->db_backend : " << this->db_backend);
    LPM_PRINT_DEBUG("this->packages_db : " << this->packages_db);
    LPM_PRINT_DEBUG("this->luas.size() : " << this->luas.size());
    for (auto& lua : this->luas) {
        LPM_PRINT_DEBUG("this->luas : " << lua.first << " " << lua.second);
    }
    LPM_PRINT_DEBUG("this->repositories.size() : " << this->repositories.size());
    for (auto& repository : this->repositories) {
        LPM_PRINT_DEBUG("this->repositories : " << repository.first);
        for (auto& source : repository.second) {
            LPM_PRINT_DEBUG("this->repositories : " << source.first << " " << source.second);
        }
    }
}

void LPM::Manifests::Config::save() {
    std::ofstream file(this->path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + this->path);
    }

    toml::value data;

    data["lpm"] = toml::value {
        {"db_backend", this->db_backend},
        {"packages_db", this->packages_db},
        {"repositories_cache", this->repositories_cache},
        {"packages_cache", this->packages_cache},
        {"modules_path", this->modules_path}
    };

    data["luas"] = toml::value{};
    for (auto& lua : this->luas) {
        data["luas"][lua.first] = lua.second;
    }

    data["sources"] = toml::value{};
    for (auto& repository : this->repositories) {
        data["sources"][repository.first] = toml::value{};
        for (auto& source : repository.second) {
            data["sources"][repository.first][source.first] = source.second;
        }
    }

    try {
        file << data;
    } catch (...) {
        throw std::runtime_error("Failed to write to file: " + this->path);
    }
}

void LPM::Manifests::Repository::load() {
    std::ifstream file(this->path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open repository file: " + this->path);
    }

    toml::value data = toml::parse(this->path);
    this->name = toml::find_or(data, "repository", "name", "");
    this->summary = toml::find_or(data, "repository", "summary", "");

    if (data.contains("packages")) {
        auto packages_tables = toml::find<
            std::map<std::string, toml::value>
        >(data, "packages");

        for (auto& package : packages_tables) {
            // Emplace back a new Repository::Package
            auto versions = toml::find<
                std::map<std::string, std::string>
            >(package.second, "versions");

            this->packages.emplace(
                package.first,
                Repository::Package {
                    package.first,
                    toml::find_or(package.second, "summary", ""),
                    toml::find_or(package.second, "package_type", ""),
                    versions
                }
            );
        }
    }


    LPM_PRINT_DEBUG("Loaded data for repository: " << this->name);

    // print all of the parsed data
    LPM_PRINT_DEBUG("this->name : " << this->name);
    LPM_PRINT_DEBUG("this->summary : " << this->summary);
    LPM_PRINT_DEBUG("this->packages.size() : " << this->packages.size());
    for (auto& package : this->packages) {
        LPM_PRINT_DEBUG("this->packages : " << package.first << " " << package.second.summary);
        LPM_PRINT_DEBUG("this->packages : " << package.first << "->package_type : " << package.second.package_type);
        // print package versions
        for (auto& version : package.second.versions) {
            LPM_PRINT_DEBUG("this->packages : " << package.first << "->versions : " << version.first << " " << version.second);
        }
    }
}

void LPM::Manifests::Repository::save() {
    std::ofstream file(this->path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open repository file: " + this->path);
    }

    toml::value data;

    data["repository"] = toml::value {
        {"name", this->name},
        {"summary", this->summary}
    };

    data["packages"] = toml::value{};
    for (auto& package : this->packages) {
        data["packages"][package.first] = toml::value {
            {"summary", package.second.summary},
            {"package_type", package.second.package_type}
        };

        data["packages"][package.first]["versions"] = toml::value{};
        for (auto& version : package.second.versions) {
            data["packages"][package.first]["versions"][version.first] = version.second;
        }
    }

    try {
        file << data;
    } catch (...) {
        throw std::runtime_error("Failed to write to file: " + this->path);
    }
}