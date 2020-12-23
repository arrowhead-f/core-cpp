#ifndef _TEST_MOCKDBASE_H_
#define _TEST_MOCKDBASE_H_

#include <iostream>

#include <initializer_list>
#include <iomanip>
#include <string>
#include <vector>
#include <utility>

#include "../../src/db/DB.h"
#include "../../src/db/DBException.h"

#include "sqlite3/sqlite3.h"


namespace {


    namespace Global {

        static std::pair<bool, std::string> result;
        static std::vector<std::vector<std::pair<bool, std::string>>> response;

    }

    int _first(void*, int argc, char **argv, char **azColName) {
        if (argv[0] == nullptr)
            Global::result = std::make_pair(false, "NULL");
        else
            Global::result = std::make_pair(true, argv[0]);
        return 1;
    }

    int _allof(void*, int argc, char **argv, char **azColName) {
        std::vector<std::pair<bool, std::string>> row;
        for(int i = 0; i < argc; i++) {
            if (argv[i] == nullptr)
                row.emplace_back(false, "NULL");
            else
                row.emplace_back(true, argv[i]);
        }
        Global::response.emplace_back(std::move(row));
        return 0;
    }

}


class MockDBase : public db::Database {

    public:


        struct DBItem : public std::pair<bool, std::string> {

            using Parent = std::pair<bool, std::string>;

            DBItem(int i) : Parent{ true, std::to_string(i) } {}
            DBItem(long l) : Parent{ true, std::to_string(l) } {}
            DBItem(double d) : Parent{ true, std::to_string(d) } {}
            DBItem(const char *s) : Parent{ true, s } {}
            DBItem(const std::string &s) : Parent{ true, s } {}
            DBItem(std::nullptr_t) : Parent{ false, "NULL" } {}
        };

        struct DBLine {

            std::vector<DBItem> line;

            DBLine(std::initializer_list<DBItem> line) : line{ line }{}
        };


    protected:

        sqlite3 *db = nullptr;

    public:

        MockDBase() {
            int rc = sqlite3_open(":memory:", &db);
            if (rc) {
                sqlite3_close(db);
                throw std::exception{};
            }
        }

        MockDBase(const char *fname) {
            int rc = sqlite3_open(fname, &db);
            if (rc) {
                sqlite3_close(db);
                throw std::exception{};
            }
        }

        MockDBase(const char *host, const char *user, const char *passwd, const char *database, unsigned port = 0, const char *connstr = "") : MockDBase{} { }

        ~MockDBase() {
            sqlite3_close(db);
        }


        bool table(const char *table, bool idx, std::initializer_list<std::string> cols, std::initializer_list<DBLine> data) {

            if (cols.size() < 1)
                return false;

            // create the table
            std::ostringstream query;
            query << "CREATE TABLE " << table << "(";

            if(!idx) {
                for(const auto &col : cols) {
                    query << col << " TEXT,";
                }
                query.seekp(-1, std::ios::end);
                query << ");";
            }
            else {
                query << *std::cbegin(cols) << " INTEGER PRIMARY KEY,";
                for(auto it = std::next(std::cbegin(cols)); it!=std::cend(cols); ++it) {
                    query << *it << " TEXT,";
                }
                query.seekp(-1, std::ios::end);
                query << ");";

            }

            int rc = sqlite3_exec(db, query.str().c_str(), nullptr, 0, nullptr);
            if (rc!=SQLITE_OK ) {
                return false;
            }

            // add rows
            for (const auto &d1 : data) {
                query.str("");
                query << "INSERT INTO " << table << " VALUES (";

                auto f = std::cbegin(d1.line);
                if(!idx) {
                    if (f->first)
                        query << std::quoted(f->second, '\'', '\'') << ",";
                    else
                        query << "NULL,";
                }
                else {
                    if (f->first)
                        query << f->second << ",";
                    else
                        query << "NULL,";
                }

                for(auto it = std::next(std::cbegin(d1.line)); it!=std::cend(d1.line); ++it) {
                    if (it->first)
                        query << "" << std::quoted(it->second, '\'', '\'') << ",";
                    else
                        query << "NULL,";
                }
                query.seekp(-1, std::ios::end);
                query << ");";

                int rc = sqlite3_exec(db, query.str().c_str(), nullptr, 0, nullptr);
                if (rc!=SQLITE_OK ) {
                    return false;
                }
            }

            return true;
        }

        bool unique(const char *table, const char *col) {
            const auto q = std::string{ "CREATE UNIQUE INDEX ux_" } + table + "_" + col + " ON " + table + "(" + col + ")";

            int rc = sqlite3_exec(db, q.c_str(), nullptr, 0, nullptr);
            if (rc!=SQLITE_OK ) {
                return false;
            }
            return true;
        }

        friend class Row;

        class Row: public db::Row {

            private:

                std::vector<std::vector<std::pair<bool, std::string>>> resp;
                std::size_t  cursor = 0;

            public:

                Row(const std::vector<std::vector<std::pair<bool, std::string>>> &resp, int status = 0) : db::Row{ resp.empty() }, resp{ resp } {}
                Row(std::vector<std::vector<std::pair<bool, std::string>>> &&resp, int status = 0) : db::Row{ resp.empty() }, resp{ std::move(resp) } {}

                ~Row() = default;

                Row(const Row&) = delete;
                Row& operator=(const Row&) = delete;
                Row(Row&&) = default;
                Row& operator=(Row&&) = default;

                bool next() final {
                    cursor++;
                    if (cursor >= resp.size()) {
                        status = 1;
                        return false;
                    }
                    return true;
                }

                bool get(unsigned column, int &result) const final {
                    const auto ret = resp.at(cursor).at(column);
                    if(!ret.first)
                        return false;

                    result = std::stoi(ret.second);
                    return true;
                }

                bool get(unsigned column, long &result) const final {
                    const auto ret = resp.at(cursor).at(column);
                    if(!ret.first)
                        return false;

                    result = std::stol(ret.second);
                    return true;
                }

                bool get(unsigned column, double &result) const final {
                    const auto ret = resp.at(cursor).at(column);
                    if(!ret.first)
                        return false;

                    result = std::stod(ret.second);
                    return true;
                }

                bool get(unsigned column, std::string &result) const final {
                    const auto ret = resp.at(cursor).at(column);
                    if(!ret.first)
                        return false;

                    result = ret.second;
                    return true;
                }
        };


        bool fetch(const char *statement, int &result) override {
            char *err;
            auto rc = sqlite3_exec(db, statement, _first, nullptr, nullptr);
            if (rc != 0 && rc != SQLITE_ABORT) {
                std::cerr << "|" << rc << "|\n";
                throw db::Exception{ 4, "Statement fetch error.444" };
            }

            if (rc != SQLITE_ABORT)
                return false;

            try {
                if (!Global::result.first)
                    return false;

                result = std::stoi(Global::result.second);

            } catch (...) {
                throw db::Exception{ 4, "Statement fetch error." };
            }

            return true;
        }

        bool fetch(const char* statement, long &result) override {
            auto rc = sqlite3_exec(db, statement, _first, nullptr, nullptr);
            if (rc != 0 && rc != SQLITE_ABORT) {
                throw db::Exception{ 4, "Statement fetch error.3333" };
            }

            if (rc != SQLITE_ABORT)
                return false;

            try {
                if (!Global::result.first)
                    return false;

                result = std::stol(Global::result.second);

            } catch (...) {
                throw db::Exception{ 4, "Statement fetch error." };
            }

            return true;
        }

        bool fetch(const char *statement, double &result) override {
            auto rc = sqlite3_exec(db, statement, _first, nullptr, nullptr);
            if (rc != 0 && rc != SQLITE_ABORT) {
                throw db::Exception{ 4, "Statement fetch error.3333" };
            }

            if (rc != SQLITE_ABORT)
                return false;

            try {
                if (!Global::result.first)
                    return false;

                result = std::stod(Global::result.second);

            } catch (...) {
                throw db::Exception{ 4, "Statement fetch error." };
            }

            return true;
        }

        bool fetch(const char *statement, std::string &result) override {
            auto rc = sqlite3_exec(db, statement, _first, nullptr, nullptr /*&errmsg*/);
            if (rc != 0 && rc != SQLITE_ABORT) {
                throw db::Exception{ 4, "Statement fetch error.555" };
            }

            if (rc != SQLITE_ABORT)
                return false;

            try {
                if (!Global::result.first)
                    return false;

                result = Global::result.second;

            } catch (...) {
                throw db::Exception{ 4, "Statement fetch error." };
            }

            return true;
        }

        std::unique_ptr<db::Row> fetch(const char *statement) override {
            Global::response.clear();

            auto rc = sqlite3_exec(db, statement, _allof, nullptr, nullptr);
            if (rc != 0) {
                throw db::Exception{ 4, "Statement fetch error.444" };
            }

            if (Global::response.empty())
                return {};

            return std::make_unique<Row>(Global::response);
        }

        void query(const char *statement) override {
            auto rc = sqlite3_exec(db, statement, nullptr, nullptr, nullptr);
            if (rc)
                throw db::Exception{ 4, "Statement fetch error.888" };
        }

        bool insert(const char *statement, int &id) override {
            auto rc = sqlite3_exec(db, statement, nullptr, nullptr, nullptr);
            if (rc)
                throw db::Exception{ 4, "Statement fetch error.888" };
            id = static_cast<int>(sqlite3_last_insert_rowid(db));
            return (id != 0);
        }

        bool insert(const char *statement, long &id) override {
            auto rc = sqlite3_exec(db, statement, nullptr, nullptr, nullptr);
            if (rc)
                throw db::Exception{ 4, "Statement fetch error.888" };
            id = static_cast<long>(sqlite3_last_insert_rowid(db));
            return (id != 0);
        }


        std::string escape(const char *str, char quote = '\'') const final {
            std::ostringstream out;
            out << std::quoted(str, quote, quote);
            return out.str();
        }


};  // class MockDBase


class MockPool {

    private:

        MockDBase &mf;

    public:

        using DatabaseType = MockDBase;

        MockPool(MockDBase &mf) : mf{ mf } { }

        MockPool(const MockPool&) = delete;
        MockPool& operator=(const MockPool&) = delete;
        MockPool(MockPool&&) = default;
        MockPool& operator=(MockPool&&) = default;

    private:

        template<typename MockDBase>friend class db::DatabaseConnection;

        std::pair<std::size_t, MockDBase*> lock() { return { 1, &mf }; }

        void unlock(std::size_t idx) { }
};

#endif  /* _TEST_MOCKDBASE_H_ */
