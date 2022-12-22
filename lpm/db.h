#pragma once
#include <sqlite3.h>
#include <string>
#include "macros.h"
#include "scope_destructor.h"

namespace LPM::DB {
    // Forward declaration
    class SQLite3;

    class Statement {
    private:
        SQLite3* owner_db;
        sqlite3_stmt* stmt;
    public:
        Statement(sqlite3_stmt* _stmt, SQLite3* _owner_db = nullptr)
        : stmt(_stmt), owner_db(_owner_db) {
            if (stmt == nullptr) {
                status = Unprepared;
                return;
            }

            status = Prepared;
        }

        Statement(const std::string& sql, SQLite3* _owner_db) {
            // Creating a new statement doesn't prepare it.
            if (_owner_db == nullptr) {
                throw std::runtime_error("Unable to prepare statement without database.");
            }
        }

        ~Statement() {
            this->finalize();
        }

        enum Status {
            Unprepared,
            Prepared,
            Closed
        };

        Status status = Unprepared;

        inline Statement& reset() {
            if (status == Prepared) {
                sqlite3_reset(stmt);
            }

            return *this;
        }

        inline Statement& finalize() {
            if (status == Prepared) {
                sqlite3_finalize(stmt);
                status = Closed;
            }

            return *this;
        }

        inline Statement& bind_text(int index, const char* value) {
            sqlite3_bind_text(stmt, index, value, -1, SQLITE_TRANSIENT);

            return *this;
        }

        inline Statement& bind_int(int index, int value) {
            sqlite3_bind_int(stmt, index, value);

            return *this;
        }

        inline Statement& bind_int64(int index, long long value) {
            sqlite3_bind_int64(stmt, index, value);

            return *this;
        }

        inline Statement& bind_double(int index, double value) {
            sqlite3_bind_double(stmt, index, value);

            return *this;
        }

        inline Statement& bind_null(int index) {
            sqlite3_bind_null(stmt, index);

            return *this;
        }

        inline Statement& bind_blob(int index, const void* value, int size) {
            sqlite3_bind_blob(stmt, index, value, size, SQLITE_TRANSIENT);

            return *this;
        }

        inline Statement& bind_zeroblob(int index, int size) {
            sqlite3_bind_zeroblob(stmt, index, size);

            return *this;
        }

        inline sqlite3_stmt*& get() {
            return stmt;
        }

        template <typename F, typename... Args>
        inline Statement& bind(F&& bind_func, Args&&... args) {
            bind_func(stmt, std::forward<Args>(args)...);

            return *this;
        }

        bool execute();

        // Takes a lambda function that takes a sqlite3_stmt* and returns nothing
        bool execute(std::function<void(sqlite3_stmt*)> callback);
    };

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

            Statement prepare(const std::string& sql);

            bool execute(LPM::DB::Statement& statement);

            template<typename F>
            bool execute(LPM::DB::Statement& statement, F callback) {
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
                    while (result = sqlite3_step(stmt)) {
                        if (result == SQLITE_ROW) {
                            callback(stmt);
                        } else {
                            break;
                        }
                    };

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

            // template<typename F>
            // bool execute(const std::string& sql, F callback) {
            //     try {
            //         sqlite3_stmt* stmt = nullptr;

            //         int result = sqlite3_prepare_v2(this->db, sql.c_str(), -1, &stmt, nullptr);

            //         if (result != SQLITE_OK) {
            //             LPM_PRINT_ERROR("SQLite3 error: " << sqlite3_errmsg(db));

            //             return false;
            //         }

            //         // Initial callback setup
            //         callback(stmt);

            //         result = sqlite3_step(stmt);

            //         while (result == SQLITE_ROW) {
            //             // Call the callback and run the next step
            //             callback(stmt);
            //             result = sqlite3_step(stmt);
            //         }

            //         if (result != SQLITE_DONE) {
            //             LPM_PRINT_ERROR("SQLite3 error: " << sqlite3_errmsg(db));

            //             return false;
            //         }

            //         LPM_PRINT_DEBUG("Successfully executed SQL");

            //         sqlite3_finalize(stmt);
            //         return true;
            //     } catch (const std::exception& e) {
            //         LPM_PRINT_ERROR("Exception occurred while trying to execute SQLite3 query: " << e.what());

            //         return false;
            //     }
            // }

        private:
            sqlite3* db;
            std::string path;
    };
}

// Inline functions that require the classes to be defined
inline bool LPM::DB::Statement::execute() {
    return owner_db->execute(*this);
}

inline bool LPM::DB::Statement::execute(
    std::function<void(sqlite3_stmt*)> callback
) {
    return owner_db->execute(*this, callback);
}