#pragma once
#include <string>
#include <map>
#include <vector>
#include "db.h"

namespace LPM::Repositories {
    std::string download_repository(
        const std::string& repository_name,
        const std::string& repository_url,
        std::string& error
    );

    void update_repository_cache(
        DB::SQLite3& db_connection,
        int repository_id,
        const std::string& repository_cache
    );

    std::vector<std::map<std::string, std::string>>
    get_repositories(DB::SQLite3& db_connection);
}