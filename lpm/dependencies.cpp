#include <sstream>
#include <curl/curl.h>
#include <zip.h>
#include "dependencies.h"
#include "manifests.h"
#include "macros.h"
#include "errors.h"
#include "repositories.h"
#include "scope_destructor.h"
#include "requests.h"
#include "env.h"
#include "utils.h"
#include "hashing.h"
#include "db.h"

LPM::Dependencies::PackageData
LPM::Dependencies::find_dependency(
    Dependency& dependency,
    std::vector<
        std::map<std::string, std::string>
    >& repositories
) {
    // Reusable error object
    std::string error;

    // Iterate through repositories and check if they have a cache
    for (auto& repository : repositories) {
        // Copy repository_cache as fill_env_+vars operates by reference
        std::string repository_cache = repository["cache"];
        Env::fill_env_vars(repository_cache);

        if (repository_cache.empty() || !Utils::fs::exists(repository_cache)) {
            LPM_PRINT_ERROR("No cache path found for repository: " << repository["name"]);
            LPM_PRINT_ERROR("Please make sure all repositories are cached before browsing them");

            return {};
        }

        LPM_PRINT_DEBUG("Loading manifest from: " << repository_cache);

        Manifests::Repository repository_manifest(repository_cache);

        // Once we have the manifest, we can check if the package we need is defined in it
        if (
            repository_manifest.packages.find(dependency.first) !=
            repository_manifest.packages.end()
        ) {
            // The package is defined in this repository,
            // so we can add it

            LPM_PRINT_DEBUG("Package " << dependency.first << " is defined in repository " << repository["name"]);

            auto& package =
                repository_manifest.packages[dependency.first];

            // Check if this package has any versions
            if (package.versions.empty()) {
                continue;
            }

            // Check if the version we need is defined in the manifest
            std::string package_version, package_manifest_url;
            if (dependency.second == "latest") {
                // Sort the keys of package.versions in descending order
                // so we can get the latest version
                std::vector<std::string> sorted_versions(package.versions.size());
                std::transform(
                    package.versions.begin(),
                    package.versions.end(),
                    sorted_versions.begin(),
                    [](const auto& pair) {
                        return pair.first;
                    }
                );

                std::sort(
                    sorted_versions.begin(),
                    sorted_versions.end(),
                    [](const auto& a, const auto& b) {
                        // Get the major, minor and patch versions
                        // of the versions
                        std::string token;
                        std::istringstream stream_a(a);
                        std::istringstream stream_b(b);

                        std::vector<std::string> tokens_a;
                        while (std::getline(stream_a, token, '.')) {
                            if (!token.empty()) {
                                tokens_a.push_back(token);
                            }
                        }

                        std::vector<std::string> tokens_b;
                        while (std::getline(stream_b, token, '.')) {
                            if (!token.empty()) {
                                tokens_b.push_back(token);
                            }
                        }

                        // Compare the major, minor and patch versions
                        // of the versions
                        size_t max_size = std::max(tokens_a.size(), tokens_b.size());
                        for (size_t i = 0; i < max_size; i++) {
                            if (tokens_a.size() - 1 < i || tokens_b.size() - 1 < i) {
                                return tokens_a.size() > tokens_b.size();
                            }

                            if (tokens_a[i] != tokens_b[i]) {
                                return std::stoi(tokens_a[i]) > std::stoi(tokens_b[i]);
                            }
                        }

                        return false;
                    }
                );

                // Get the latest version
                const auto& latest_version = sorted_versions.front();
                package_manifest_url = package.versions[latest_version];
                package_version = latest_version;

                LPM_PRINT_DEBUG(
                    "Latest version of " << dependency.first << ":"
                    << latest_version
                );
            } else {
                // Check if the dependency version is defined in the package
                if (package.versions.find(dependency.second) == package.versions.end()) {
                    continue;
                }

                package_manifest_url = package.versions[dependency.second];
                package_version = dependency.second;

                LPM_PRINT_DEBUG("Version " << dependency.second << " of " << dependency.first << " is defined");
            }

            return PackageData(
                package.name,
                package_version,
                package_manifest_url,
                repository["id"]
            );
        }
    }

    return {};
}

bool LPM::Dependencies::insert_into_db(
    DB::SQLite3& db_connection,
    const std::string& package_hash,
    PackageData& package_info
) {
    // Insert a package into the database
    LPM_PRINT_DEBUG("Inserting package " << package_info.name << ":" << package_hash << " into database");
    db_connection.prepare(
        "INSERT INTO packages (id, name, version, repository) VALUES (?, ?, ?, ?);"
    )
    .bind_text(1, package_hash.c_str())
    .bind_text(2, package_info.name.c_str())
    .bind_text(3, package_info.version.c_str())
    .bind_int(4, package_info.repository_id)
    .execute();

    return true;
}

bool LPM::Dependencies::add(
    LPM::DB::SQLite3& db_connection,
    PackageData& package,

    std::string cache_path,
    std::string package_path,
    std::string& error
) {
    // TODO: Download, unpack and add the dependency
    // then add it to our list of added dependencies in
    // the packages db
    LPM_PRINT_DEBUG(
        "Adding dependency " <<
        package.name << ":" <<
        package.version
    );

    if (package.package_type == "zip") {
        Requests::Response response;

        try {
            scope_destructor<CURL*> curl_handle(curl_easy_init(), curl_easy_cleanup);
            if (!curl_handle.get()) {
                error = "Failed to initialize curl";

                return false;
            }

            response = Requests::get(package.package_url, curl_handle.get());

            if (response.status_code != 200) {
                error =
                    "Failed to download package from url '" +
                    package.package_url + "': " +
                    std::to_string(response.status_code);

                return false;
            }
        } catch (const std::exception& e) {
            error =
                "Exception occurred while trying to download package from url '" +
                package.package_url + "': " +
                e.what();

            return false;
        }

        // Save the package to the cache
        if (!Utils::write_file(cache_path, response.body)) {
            error = "Failed to save package cache to " + cache_path;

            return false;
        } else {
            LPM_PRINT_DEBUG("Saved package cache to " << cache_path);
        }

        try {
            // Read the package
            int error_code = 0;
            scope_destructor<zip*> manifest_zip(
                zip_open(cache_path.c_str(), 0, &error_code),
                zip_close
            );

            if (!manifest_zip.get()) {
                error =
                    "Can't open zip file '" + cache_path + "' " +
                    "(libzip error code: " + std::to_string(error_code) + ")";

                // Cancel manifest_zip destructor to prevent
                // zip_close from being called
                manifest_zip.cancel();

                return false;
            }

            if (!Utils::unzip(manifest_zip.get(), package_path, error)) {
                error =
                    "Failed to extract package " + package_path + " (" + error + ")";

                return false;
            }
        } catch (const std::exception& e) {
            error = "Exception occurred while trying to extract package at '" + cache_path + "': " + e.what();

            return false;
        }
    }

    // Add the package to the database

    // Get the package hash
    std::string package_hash = Hashing::djb2(package_path);

    // Add the package to the database
    insert_into_db(db_connection, package_hash, package);

    return true;
}


bool LPM::Dependencies::is_added(
    DB::SQLite3& db_connection,
    const std::string& package_path
) {
    // Check if the directory at package_path exists
    // This is done like this because users can manually
    // install a package in the expected location if they're
    // desired to do so (even if it's not recommended)
    LPM_PRINT_DEBUG("Checking if package at " << package_path << " is added");
    bool is_physically_installed = false;
    if (Utils::fs::exists(package_path)) {
        LPM_PRINT_DEBUG("Package at " << package_path << " is added");
        is_physically_installed = true;
    }

    // Check if the package is already added to the database
    std::string package_hash = Hashing::djb2(package_path);

    // todo:: try using EXISTS() function instead of a raw select 1
    bool is_in_db = false;
    db_connection.prepare(
        "SELECT 1 FROM packages WHERE id = ?;"
    )
    .bind_text(1, package_hash.c_str())
    .execute([&](sqlite3_stmt* stmt) {
        // Lambda is only called when a row was found
        is_in_db = true;
    });

    LPM_PRINT_DEBUG("Package " << package_path << " is " << (is_in_db ? "already" : "not") << " added to the db");

    if (is_in_db && !is_physically_installed) {
        // This package is in the db but not physically installed,
        // so we should remove it from the db
        LPM_PRINT_DEBUG("Removing package " << package_path << " from db");
        db_connection.prepare(
            "DELETE FROM packages WHERE id = ?;"
        )
        .bind_text(1, package_hash.c_str())
        .execute();

        return false;
    }

    return is_in_db;
}