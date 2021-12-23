#ifndef _DB_MARIADB_H_
#define _DB_MARIADB_H_


#include <iostream>

#include <string>
#include <cstring>
#include <cctype>

#include <mariadb/mysql.h>

#include "DB.h"

namespace db {

    namespace {

        template<typename T>struct MariaDBBuffer {
            static_assert(sizeof(T)==0, "Undefined MySQLBuffer type.");
        };

        template<> struct MariaDBBuffer<int> {
            static constexpr auto Type = MYSQL_TYPE_LONG;
            static inline int convert(const char *val) { return std::atoi(val); }
        };

        #if LONG_MAX >= 9223372036854775807
            template<> struct MariaDBBuffer<long> {
                static constexpr auto Type = MYSQL_TYPE_LONGLONG;
                static inline long convert(const char *val) { return std::atol(val); }
            };
        #else
            template<> struct MariaDBBuffer<long> {
                static constexpr auto Type = MYSQL_TYPE_LONG;
                static inline long convert(const char *val) { return std::atol(val); }
            };
        #endif

        template<> struct MariaDBBuffer<double> {
            static constexpr auto Type = MYSQL_TYPE_DOUBLE;
            static inline double convert(const char *val) { return std::stod(val); }
        };

        template<> struct MariaDBBuffer<std::string> {
            static constexpr auto Type = MYSQL_TYPE_STRING;
            static inline std::string convert(const char *val) { return std::string{ val }; }
        };

        template<typename T>inline bool fetch_impl(MYSQL *mysql, const char *statement, T &result) {

            if (mysql_query(mysql, statement))
                throw db::Exception{ 1, "Query error." };  // not enough memory

            MYSQL_RES *res = mysql_store_result(mysql);


            if (!res)
                throw db::Exception{ 1, "Cannot store result." };  // not enough memory

            int num_fields = mysql_num_fields(res);
            if(num_fields < 1) {
                mysql_free_result(res);
                throw db::Exception{ 1, "Wrong field number." };  // not enough memory
            }

            MYSQL_ROW row;

            while ((row = mysql_fetch_row(res))) {

                if(!row[0])
                    break;

                result = MariaDBBuffer<T>::convert(row[0]);
                mysql_free_result(res);
                return true;
            }

            mysql_free_result(res);
            return false;
        }

        template<typename T>inline bool get_impl(MYSQL_STMT *stmt, unsigned column, T &result) {

            MYSQL_BIND bind;
            std::memset(&bind, 0, sizeof(bind));

            char is_null, error;
            unsigned long length;

            result = 0;

            bind.buffer_type = MariaDBBuffer<T>::Type;
            bind.buffer  = (char *)&result;
            bind.is_null = &is_null;
            bind.length  = &length;
            bind.error   = &error;

            if(mysql_stmt_fetch_column(stmt, &bind, column, 0) == 1) {
                return false;
            }

            if(is_null)
                return false;

            return true;
        }

    }  // namespace

    class MariaDB final: public Database {

    private:

        MYSQL *mysql = nullptr;  ///< The database handler.

        // constants:
        static constexpr int STRING_BUFFER_LEN = 127;   ///< Default buffer length for string queries.
        mutable char escbuffer[STRING_BUFFER_LEN + 1] = { 0 };  ///< To avoid frequent allocations this buffer is used for escaping strings.

    public:

        static constexpr const char *bind = "?";

        class Row: public db::Row {
            private:
                MYSQL_STMT *stmt;                  ///< The statement encapsulated in the row.
                bool       result_stored = false;  ///< Whether the result was stored on client.

            public:

                Row(MYSQL_STMT *stmt, bool store_result = true) : db::Row{ (store_result?mysql_stmt_store_result(stmt):0, mysql_stmt_fetch(stmt)) }, stmt{ stmt }, result_stored{ store_result } {
                    if(status != 0 && status != MYSQL_NO_DATA) {
                        if(result_stored)
                            mysql_stmt_free_result(stmt);
                        mysql_stmt_close(stmt);
                        stmt = nullptr;
                        throw Exception{ -1, "Statement fetch error", mysql_stmt_errno(stmt) };
                    }
                }

                ~Row(){
                    if(stmt) {
                        if(result_stored)
                            mysql_stmt_free_result(stmt);
                        mysql_stmt_close(stmt);
                    }
                }

                Row(const Row&) = delete;
                Row& operator=(const Row&) = delete;
                Row(Row&&) = default;
                Row& operator=(Row&&) = default;

                bool next() final {
                    switch(status = mysql_stmt_fetch(stmt)){
                        case 0:
                            return true;
                        case MYSQL_NO_DATA:
                            return false;
                    }
                    throw Exception{ -1, "Statement fetch error.", mysql_stmt_errno(stmt) };
                }

                bool get(unsigned column, int &result) const final {
                    return get_impl(stmt, column, result);
                }

                bool get(unsigned column, long &result) const final {
                    return get_impl(stmt, column, result);
                }

                bool get(unsigned column, double &result) const final {
                    return get_impl(stmt, column, result);
                }

                bool get(unsigned column, std::string &result) const final {
                    MYSQL_BIND bind;
                    std::memset(&bind, 0, sizeof(bind));

                    char is_null, error;
                    unsigned long length;
                    char buffer[STRING_BUFFER_LEN + 1];

                    bind.buffer_type   = MYSQL_TYPE_STRING;
                    bind.buffer        = buffer;
                    bind.is_null       = &is_null;
                    bind.length        = &length;
                    bind.error         = &error;
                    bind.buffer_length = STRING_BUFFER_LEN;

                    result.clear();
                    unsigned offset = 0;
                    do {
                        if (mysql_stmt_fetch_column(stmt, &bind, column, offset))
                            return false;

                        if (is_null)
                            return false;

                        result += buffer;
                        offset += STRING_BUFFER_LEN;
                    } while(offset < length);

                    if(is_null)
                        return false;

                    return true;
                }
            };

            MariaDB(const char *host, const char *user, const char *passwd, const char *database, unsigned port = 0, const char *connstr = "") : mysql{mysql_init(nullptr) } {
                if (!mysql)
                    throw db::Exception{ 1, "Not enough memory." };  // not enough memory

                //mysql_options(mysql, MYSQL_READ_DEFAULT_GROUP, "your_prog_name");
                if (!mysql_real_connect(mysql, host, user, passwd, database, port, nullptr, 0)) {
                    throw db::Exception{ 1, "Cannot connect to database." };
                }
            }

            ~MariaDB() {
                mysql_close(mysql);
            }

            bool fetch(const char *statement, int &result) final {
                return fetch_impl(mysql, statement, result);
            }

            bool fetch(const char* statement, long &result) final {
                return fetch_impl(mysql, statement, result);
            }

            bool fetch(const char *statement, double &result) final {
                return fetch_impl(mysql, statement, result);
            }

            bool fetch(const char *statement, std::string &result) final {
                return fetch_impl(mysql, statement, result);
            }

            std::unique_ptr<db::Row> fetch(const char *statement) final {
                MYSQL_STMT *stmt = mysql_stmt_init(mysql);

                if (!stmt) {
                    throw Exception{ -1, "Cannot initialize a new statement.", mysql_stmt_errno(stmt), mysql_stmt_error(stmt) };
                }

                if (mysql_stmt_prepare(stmt, statement, std::strlen(statement))) {
                    throw Exception{ -1, "Cannot prepare the statement.", mysql_stmt_errno(stmt), mysql_stmt_error(stmt) };
                }

                if (!mysql_stmt_execute(stmt)) {
                    auto *p = new MariaDB::Row(stmt, true);

                    // only return the row, if there's (at least) one in the result set
                    if(static_cast<bool>(*p))
                        return std::unique_ptr<db::Row>{ p };
                }

                return {};
            }

            void query(const char *statement) final {
                if (mysql_query(mysql, statement))
                    throw Exception{-1, "Query failed.", mysql_errno(mysql), mysql_error(mysql) };
            }

            std::size_t query_and_check(const char *statement) final {
                if (mysql_query(mysql, statement))
                    throw Exception{-1, "Query failed.", mysql_errno(mysql), mysql_error(mysql) };
                return mysql_affected_rows(mysql);
            }

            bool insert(const char *statement, int &id) final {
                if (mysql_query(mysql, statement))
                    throw Exception{-1, "Query failed.", mysql_errno(mysql), mysql_error(mysql) };

                if (mysql_store_result(mysql) == 0 && mysql_field_count(mysql) == 0 && mysql_insert_id(mysql) != 0) {
                    id = mysql_insert_id(mysql);
                    return true;
                }

                return false;
            }

            bool insert(const char *statement, long &id) final {
                if (mysql_query(mysql, statement))
                    throw Exception{-1, "Query failed.", mysql_errno(mysql), mysql_error(mysql) };

                if (mysql_store_result(mysql) == 0 && mysql_field_count(mysql) == 0 && mysql_insert_id(mysql) != 0) {
                    id = mysql_insert_id(mysql);
                    return true;
                }

                return false;
            }

            std::string escape(const char *str, char quote = '\'') const final {
                const auto len = std::strlen(str);

                if(STRING_BUFFER_LEN >= 2 * len) {
                    mysql_real_escape_string(mysql, escbuffer, str, len);
                    return { escbuffer };
                }

                // sadly enough, but we have to allocate a new array for conversion
                char *buffer = new char[2 * len + 1];

                //mysql_real_escape_string_quote(mysql, buffer, str, len, quote);
                mysql_real_escape_string(mysql, buffer, str, len);  // in mariadb howto escape table name and column names??

                std::string result{ buffer };
                delete[] buffer;

                return result;
            }

            std::string escape(const std::string &str, char quote = '\'') const final {
                const auto len = str.length();

                if(STRING_BUFFER_LEN >= 2 * len) {
                    mysql_real_escape_string(mysql, escbuffer, str.c_str(), len);
                    return { escbuffer };
                }

                // sadly enough, but we have to allocate a new array for conversion
                char *buffer = new char[2 * len + 1];

                //mysql_real_escape_string_quote(mysql, buffer, str, len, quote);
                mysql_real_escape_string(mysql, buffer, str.c_str(), len);  // in mariadb howto escape table name and column names??

                std::string result{ buffer };
                delete[] buffer;

                return result;
            }

            void begin() final {
                if (mysql_query(mysql, "START TRANSACTION"))
                    throw Exception{ -1, "Starting transaction failed.", mysql_errno(mysql), mysql_error(mysql) };
            }

            void rollback() final {
                if (mysql_query(mysql, "ROLLBACK"))
                    throw Exception{ -1, "Transaction rollback failed.", mysql_errno(mysql), mysql_error(mysql) };
            }

            void commit() final {
                if (mysql_query(mysql, "COMMIT"))
                    throw Exception{ -1, "Transaction commit failed.", mysql_errno(mysql), mysql_error(mysql) };
            }

    };  // class MariaDB

}  // namespace db

#endif  /* _DB_MARIADB_H_ */
