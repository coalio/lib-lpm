#include "manifests.h"
#include "macros.h"
#include "toml11/toml.hpp"

void LPM::Manifests::Packages::load() {
    toml::value data;
    std::ifstream file(this->path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open package manifest file: " + this->path);
    }

    try {
        data = toml::parse(this->path);
    } catch (...) {
        throw std::runtime_error("Failed to parse: " + this->path);
    }

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

void LPM::Manifests::Config::load() {
    toml::value data;
    std::ifstream file(this->path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + this->path);
    }

    try {
        data = toml::parse(this->path);
    } catch (...) {
        throw std::runtime_error("Failed to parse: " + this->path);
    }

    this->db_backend = toml::find_or(data, "lpm", "db_backend", "");
    this->packages_db = toml::find_or(data, "lpm", "packages_db", "");
    this->repositories_cache = toml::find_or(data, "lpm", "repositories_cache", "");

    std::string missing_values = "";

    if (this->db_backend == "") {
        missing_values = "db_backend";
    }

    if (this->packages_db == "") {
        missing_values += ", packages_db";
    }

    if (this->repositories_cache == "") {
        missing_values += ", repositories_cache";
    }

    if (missing_values != "") {
        throw std::runtime_error("Missing values in " + this->path + ": " + missing_values);
    }

    if (data.contains("luas")) {
        this->luas = toml::find<
            std::map<std::string, std::string>
        >(data, "luas");
    } else {
        throw std::runtime_error("No Lua interpreters specified in the lpm.toml file.");
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

void LPM::Manifests::Repository::load() {
    toml::value data;
    std::ifstream file(this->path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open repository file: " + this->path);
    }

    try {
        data = toml::parse(this->path);
    } catch (...) {
        throw std::runtime_error("Failed to parse: " + this->path);
    }

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