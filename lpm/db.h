#pragma once
#include <sqlite3.h>
#include <string>
#include "macros.h"
#include "scope_destructor.h"

namespace LPM::DB {
    class SQLite3 {
        public:
            SQLite3() = default;

            SQLite3(const std::string& path) {
                if (!this->open(path)) {
                    throw std::runtime_error("Unable to open database at path: " + path);
                };
            }

            // Move constructor
            SQLite3(SQLite3&& other) {
                this->db = other.db;
                this->path = other.path;
                other.db = nullptr;
            }

            ~SQLite3() {
                if (this->db == nullptr || !this->is_open) {
                    return;
                }

                if (!this->close()) {
                    // This is strange, but we can't throw an error here
                    // because we're in a destructor.
                    // So, lets just swallow the memory leak.
                    LPM_PRINT_ERROR("Unable to close database at path: " + path);
                }
            }

            bool is_open = false;

            bool open(const std::string& path);
            bool close();

            bool execute(const std::string& sql);

            template<typename F>
            bool execute(const std::string& sql, F callback) {
                try {
                    sqlite3_stmt* stmt = nullptr;

                    int result = sqlite3_prepare_v2(this->db, sql.c_str(), -1, &stmt, nullptr);

                    if (result != SQLITE_OK) {
                        LPM_PRINT_ERROR("SQLite3 error: " << result);

                        return false;
                    }

                    result = sqlite3_step(stmt);

                    while (result == SQLITE_ROW) {
                        // Call the callback and run the next step
                        callback(stmt);
                        result = sqlite3_step(stmt);
                    }

                    if (result != SQLITE_DONE) {
                        LPM_PRINT_ERROR("SQLite3 error: " << result);

                        return false;
                    } else {
                        sqlite3_finalize(stmt);
                    }

                    return true;
                } catch (const std::exception& e) {
                    LPM_PRINT_ERROR("Exception occurred while trying to execute SQLite3 query: " << e.what());

                    return false;
                }
            }

            template<typename F>
            bool execute(const std::string& sql, F callback) {
                try {
                    sqlite3_stmt* stmt = nullptr;

                    int result = sqlite3_prepare_v2(this->db, sql.c_str(), -1, &stmt, nullptr);

                    if (result != SQLITE_OK) {
                        LPM_PRINT_ERROR("SQLite3 error: " << result);

                        return false;
                    }

                    // Initial callback setup
                    callback(stmt);

                    result = sqlite3_step(stmt);

                    while (result == SQLITE_ROW) {
                        // Call the callback and run the next step
                        callback(stmt);
                        result = sqlite3_step(stmt);
                    }

                    if (result != SQLITE_DONE) {
                        LPM_PRINT_ERROR("SQLite3 error: " << result);

                        return false;
                    } else {
                        sqlite3_finalize(stmt);
                    }

                    return true;
                } catch (const std::exception& e) {
                    LPM_PRINT_ERROR("Exception occurred while trying to execute SQLite3 query: " << e.what());

                    return false;
                }
            }

        private:
            sqlite3* db;
            std::string path;
    };
}