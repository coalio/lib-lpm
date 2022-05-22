#include "db.h"

bool LPM::DB::SQLite3::open(const std::string& path) {
    if (this->is_open) {
        return false;
    }

    this->path = path;
    this->is_open = (
        sqlite3_open(path.c_str(), &this->db) == SQLITE_OK
    );

    return this->is_open;
}

bool LPM::DB::SQLite3::close() {
    if (!this->is_open) {
        return false;
    }

    int result = sqlite3_close(this->db);
    bool success = (result == SQLITE_OK);
    this->is_open = !(
        success
    );

    LPM_PRINT_DEBUG("closed database at path: " << path << " with result " << !this->is_open << " or also " << result);
    return !this->is_open;
}

bool LPM::DB::SQLite3::execute(const std::string& sql) {
    try {
        scope_destructor<char*> error(nullptr, sqlite3_free);
        int result = sqlite3_exec(this->db, sql.c_str(), nullptr, nullptr, &error.get());
        LPM_PRINT_DEBUG("Result: " << result);
        if (result != SQLITE_OK) {
            LPM_PRINT_ERROR("SQLite3 error: " << error.get());

            return false;
        }

        LPM_PRINT_DEBUG("Successfully executed SQL");

        return true;
    } catch (const std::exception& e) {
        LPM_PRINT_ERROR("Exception occurred while trying to execute SQLite3 query: " << e.what());

        return false;
    }
}