#pragma once
#include <string>
#include <utility>
#include "manifests.h"
#include "db.h"

using namespace LPM::Manifests;
using namespace LPM::DB;

namespace LPM::Dependencies {
    typedef std::pair<std::string, std::string> Dependency;

    // Contains package information necessary for installation
    class PackageData {
    public:
        std::string name;
        std::string version;
        std::string manifest_url;
        std::string package_url;
        std::string package_type;
        int repository_id;

        PackageData(
            const std::string& _name,
            const std::string& _version,
            const std::string& _manifest_url,
            std::string _repository_id
        ) :
            name(_name),
            version(_version),
            manifest_url(_manifest_url),
            repository_id(std::stoi(_repository_id))
        {}

        PackageData() = default;
    };

    PackageData find_dependency(
        Dependency& dependency,
        std::vector<
            std::map<std::string, std::string>
        >& repositories
    );

    bool insert_into_db(
        DB::SQLite3& db_connection,
        const std::string& package_hash,
        PackageData& package_info
    );

    bool add(
        DB::SQLite3& db_connection,
        PackageData& package,
        std::string cache_path,
        std::string package_path,
        std::string& error
    );

    bool is_added(
        DB::SQLite3& db_connection,
        const std::string& package_path
    );

    bool insert_into_db(
        DB::SQLite3& db_connection,
        const std::string& package_hash,
        PackageData& package_info
    );
}