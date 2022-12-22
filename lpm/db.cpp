#include "db.h"

using LPM::DB::Statement;
using LPM::DB::SQLite3;

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

Statement LPM::DB::SQLite3::prepare(const std::string& sql) {
    if (!this->is_open) {
        throw std::runtime_error("Unable to prepare statement in closed database.");
    }

    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(this->db, sql.c_str(), -1, &stmt, nullptr);

    if (result != SQLITE_OK) {
        throw std::runtime_error("Unable to prepare statement: " + std::string(sqlite3_errmsg(this->db)));
    }

    return Statement(stmt, this);
}

bool LPM::DB::SQLite3::execute(LPM::DB::Statement& statement) {
    try {
        switch (statement.status) {
            case Statement::Closed:
                throw std::runtime_error("Unable to execute closed statement.");
            case Statement::Unprepared:
                throw std::runtime_error("Unable to execute unprepared statement.");
            default:
                break;
        }

        sqlite3_stmt* stmt = statement.get();

        int result;
        do {
            result = sqlite3_step(stmt);
        } while (result == SQLITE_ROW);

        if (result != SQLITE_DONE) {
            LPM_PRINT_ERROR("SQLite3 error: " << sqlite3_errmsg(db));

            return false;
        }

        LPM_PRINT_DEBUG("Successfully executed SQL");

        statement.finalize();
        return true;
    } catch (const std::exception& e) {
        LPM_PRINT_ERROR("Exception occurred while trying to execute SQLite3 query: " << e.what());

        return false;
    }
}