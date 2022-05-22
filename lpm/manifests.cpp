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

    if (this->lua_version.empty()) {
        throw std::runtime_error(
            "No project.lua_version field in your project manifest"
        );
    }

    if (data.contains("dependencies")) {
        this->dependencies = toml::find<
            std::map<std::string, std::string>
        >(data, "dependencies");
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

void LPM::Manifests::Package::load() {
    std::ifstream file(this->path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open package manifest file: " + this->path);
    }

    toml::value data = toml::parse(this->path);
    this->name = toml::find_or(data, "package", "name", "");
    this->version = toml::find_or(data, "package", "version", "");
    this->description = toml::find_or(data, "package", "description", "");
    this->license = toml::find_or(data, "package", "license", "");
    this->repository = toml::find_or(data, "package", "repository", "");
    this->maintainer = toml::find_or(data, "package", "maintainer", "");

    if (data.contains("source")) {
        this->source.url = toml::find_or(data, "source", "url", "");
        this->source.package_type = toml::find_or(data, "source", "package_type", "");
    }

    if (data.contains("run")) {
        this->run.main = toml::find_or(data, "run", "main", "");
        this->run.lua_version = toml::find<std::vector<std::string>>(data, "run", "lua_version");
    }

    if (data.contains("support")) {
        this->support.lua_version = toml::find<std::vector<std::string>>(data, "support", "lua_version");
        this->support.platform = toml::find<std::vector<std::string>>(data, "support", "platform");
    }

    if (data.contains("dependencies")) {
        this->dependencies = toml::find<
            std::map<std::string, std::string>
        >(data, "dependencies");
    }
}

void LPM::Manifests::Package::save() {
    std::ofstream file(this->path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open package manifest file: " + this->path);
    }

    toml::value data;

    data["package"] = toml::value {
        {"name", this->name},
        {"version", this->version},
        {"description", this->description},
        {"license", this->license},
        {"repository", this->repository},
        {"maintainer", this->maintainer}
    };

    if (this->run.main.size() > 0) {
        data["run"] = toml::value {
            {"main", this->run.main},
            {"lua_version", this->run.lua_version}
        };
    }

    if (this->support.lua_version.size() > 0) {
        data["support"] = toml::value {
            {"lua_version", this->support.lua_version},
            {"platform", this->support.platform}
        };
    }

    try {
        file << data;
    } catch (...) {
        throw std::runtime_error("Failed to write to file: " + this->path);
    }
}

void LPM::Manifests::Config::load(bool ignore_missing) {
    std::ifstream file(this->path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + this->path);
    }

    toml::value data = toml::parse(this->path);
    this->db_backend = toml::find_or(data, "lpm", "db_backend", "");
    this->packages_db = toml::find_or(data, "lpm", "packages_db", "");
    this->repositories_cache = toml::find_or(data, "lpm", "repositories_cache", "");
    this->packages_cache = toml::find_or(data, "lpm", "packages_cache", "");
    this->packages_path = toml::find_or(data, "lpm", "packages_path", "");

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

    if (this->packages_path == "") {
        missing_keys += " packages_path";
    }

    if (missing_keys != "" && !ignore_missing) {
        throw std::runtime_error(
            "The 'lpm' section in your lpm.toml configuration file is missing the following keys:" + missing_keys
        );
    }

    if (data.contains("luas")) {
        this->luas = toml::find<
            std::map<std::string, std::string>
        >(data, "luas");

        if (!this->luas.contains("default") && !ignore_missing) {
            throw std::runtime_error("You must specify a 'default' Lua interpreter in your lpm.toml file, under 'luas' section.");
        }
    } else if (!ignore_missing) {
        throw std::runtime_error("No Lua interpreters specified in the lpm.toml file. Specify them using the 'luas' section");
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
        {"packages_path", this->packages_path}
    };

    data["luas"] = toml::value{};
    for (auto& lua : this->luas) {
        data["luas"][lua.first] = lua.second;
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
                    versions
                }
            );
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
            {"summary", package.second.summary}
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