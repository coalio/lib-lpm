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
    };

    class Config {
    public:
        Config(const std::string& path) {
            this->path = path;
            this->load();
        }

        std::string
            path, db_backend, packages_db;

        std::map<std::string, std::string> luas;
        std::map<
            std::string,
            std::map<std::string, std::string>
        > repositories;

        void load();
    };

    class Repository {
    private:
        struct Package {
            Package(
                std::string _name,
                std::string _summary,
                std::string _package_type,
                std::map<std::string, std::string> _versions
            ) : name(_name),
                summary(_summary),
                package_type(_package_type),
                versions(_versions) {}

            std::string name, summary, package_type;
            std::map<std::string, std::string> versions;
        };
    public:
        Repository(const std::string& path) {
            this->path = path;
            this->load();
        }

        std::string
            path, name, summary;

        std::map<std::string, Repository::Package> packages;

        void load();
    };
}