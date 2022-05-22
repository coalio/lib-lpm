#pragma once
#include <string>
#include <vector>
#include <map>

namespace LPM::Manifests {
    class Packages {
    public:
        Packages(const std::string& path) {
            this->path = path;
            this->load();
        }

        std::string
            path, name, version,
            description, author, license,
            homepage, repository, main,
            lua_version;

        std::map<std::string, std::string> dependencies;

        void load();
        void save();
    };

    class Package {
    public:
        Package(const std::string& path) {
            this->path = path;
            this->load();
        }

        std::string
            path, name, version,
            description, license, repository,
            maintainer;

        struct {
            std::string url;
            std::string package_type;
        } source;

        struct {
            std::string main;
            std::vector<std::string> lua_version;
        } run;

        struct {
            std::vector<std::string> lua_version;
            std::vector<std::string> platform;
        } support;

        std::map<std::string, std::string> dependencies;

        void load();
        void save();
    };

    class Config {
    public:
        Config() = default;
        Config(const std::string& path) {
            this->path = path;
            this->load();
        }

        std::string
            path, db_backend, packages_db,
            repositories_cache, packages_cache,
            packages_path;

        std::map<std::string, std::string> luas;

        void load(bool ignore_missing = false);
        void save();
    };

    class Repository {
    public:
        struct Package {
            Package() = default;

            Package(
                std::string _name,
                std::string _summary,
                std::map<std::string, std::string> _versions
            ) : name(_name),
                summary(_summary),
                versions(_versions) {}

            std::string name, summary;
            std::map<std::string, std::string> versions;

            // Move constructor
            Package(Package&& other) {
                this->name = std::move(other.name);
                this->summary = std::move(other.summary);
                this->versions = std::move(other.versions);
            }
        };

        Repository(const std::string& path) {
            this->path = path;
            this->load();
        }

        std::string
            path, name, summary;

        std::map<std::string, Repository::Package> packages;

        void load();
        void save();
    };
}