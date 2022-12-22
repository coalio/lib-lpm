#include <curl/curl.h>
#include "repositories.h"
#include "scope_destructor.h"
#include "requests.h"

std::string LPM::Repositories::download_repository(
    const std::string& repository_name,
    const std::string& repository_url,
    std::string& error
) {
    Requests::Response response;

    try {
        // Initialize a new curl handle and give it a scope_destructor, so
        // we can call curl_easy_cleanup() on it when it goes out of scope
        scope_destructor<CURL*> curl_handle(curl_easy_init(), curl_easy_cleanup);
        response = Requests::get(repository_url, curl_handle.get());

        if (response.status_code != 200) {
            error =
                "(" + repository_name + ") cant download from: " + repository_url +
                ", status code " + std::to_string(response.status_code);

            return "";
        }

        if (response.body.empty()) {
            error = "(" + repository_name + ") empty response";

            return "";
        }
    } catch (const std::exception& e) {
        error =
            "(" + repository_name + ") cant download from: " + repository_url +
            ", error: " + e.what();

        return "";
    }

    return response.body;
}

std::vector<std::map<std::string, std::string>>
LPM::Repositories::get_repositories(DB::SQLite3& db_connection) {
    // Get all of the repositories from the database
    // reusing the same database connection
    std::vector<std::map<std::string, std::string>> repositories;

    db_connection.prepare(
        "SELECT * FROM repositories;"
    )
    .execute(
        [&] (sqlite3_stmt* result) {
            int column_count = sqlite3_column_count(result);
            repositories.emplace_back();

            // For every column, assign the value to the map
            for (int i = 0; i < column_count; i++) {
                const char* column_name_c_str = const_cast<char*>(sqlite3_column_name(result, i));
                const char* column_value_c_str = reinterpret_cast<const char*>(sqlite3_column_text(result, i));

                if (column_name_c_str != NULL && column_value_c_str != NULL) {
                    repositories.back()[column_name_c_str] = column_value_c_str;
                } else if (column_name_c_str != NULL) {
                    repositories.back()[column_name_c_str] = "";
                }
            }
        }
    );

    return repositories;
}

void LPM::Repositories::update_repository_cache(
    DB::SQLite3& db_connection,
    int repository_id,
    const std::string& repository_cache
) {
    // Update the repository cache
    db_connection.prepare(
        "UPDATE repositories SET cache = ? WHERE id = ?;"
    )
    .bind_text(1, repository_cache.c_str())
    .bind_int(2, repository_id)
    .execute();
}