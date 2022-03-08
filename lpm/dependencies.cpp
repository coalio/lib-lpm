#include <sstream>
#include <curl/curl.h>
#include <zip.h>
#include "dependencies.h"
#include "manifests.h"
#include "macros.h"
#include "scope_destructor.h"
#include "requests.h"
#include "env.h"
#include "utils.h"

using namespace LPM::Dependencies;

bool LPM::Dependencies::is_installed(const Dependency& dependency) {
    // TODO: Return true if dependency is installed
    // We must take into account the version of the dependency, so
    // if its "latest" we must check if our installed version is
    // the latest version.

    return false;
}

bool LPM::Dependencies::install(
    const Dependency& dependency,
    Repository::Package& package,
    std::string cache_path,
    std::string module_path,
    std::string& error
) {
    // TODO: Download, unpack and install the dependency
    // then add it to our list of installed dependencies in
    // the packages db
    LPM_PRINT_DEBUG(
        "Installing dependency " <<
        dependency.first << ":" <<
        dependency.second
    );

    // Check if the dependency is already installed
    if (is_installed(dependency)) {
        error = "Dependency " + dependency.first + ":" + dependency.second + " is already installed";

        return false;
    }

    // Declare the package url
    std::string package_url = package.versions[dependency.second];

    if (package.package_type == "zip") {
        Requests::Response response;

        try {
            scope_destructor<CURL*> curl_handle(curl_easy_init(), curl_easy_cleanup);
            if (!curl_handle.get()) {
                error = "Failed to initialize curl";

                return false;
            }

            response = Requests::get(package_url, curl_handle.get());

            if (response.status_code != 200) {
                error =
                    "Failed to download package from url '" + package_url + "': " + std::to_string(response.status_code);

                return false;
            }
        } catch (const std::exception& e) {
            error = "Exception occurred while trying to download package from url '" + package_url + "': " + e.what();

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

            if (!Utils::unzip(manifest_zip.get(), module_path, error)) {
                error =
                    "Failed to extract package " + module_path + " (" + error + ")";

                return false;
            }
        } catch (const std::exception& e) {
            error = "Exception occurred while trying to read package at '" + cache_path + "': " + e.what();

            return false;
        }
    }

    return true;
}
