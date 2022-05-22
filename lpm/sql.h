// A collection of SQL macros

#define SQL_SELECT(table, columns, where) \
    "SELECT " columns " FROM " table " WHERE " where

#define SQL_SELECT_ALL(table) \
    "SELECT * FROM " table