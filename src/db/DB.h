#ifndef _DB_DB_H_
#define _DB_DB_H_


#include <algorithm>
#include <stack>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <functional>
#include <type_traits>
#include <thread>

#include "DBException.h"

namespace db {

    class Database;

    /// The database pool.
    template<typename DB>
    class DatabasePool {
        private:

            std::mutex mux;              ///< Mutex to access the shared state.
            std::condition_variable cv;  ///<

            std::stack<std::size_t>          idxs;   ///< The stack of indexes.
            std::vector<std::unique_ptr<DB>> pool;   ///< The pool of database connections.

        public:

            using DatabaseType = DB;

            DatabasePool(const char *host, const char *user, const char *passwd, const char *database, unsigned port = 0, const char *connstr = "") {
                for(std::size_t i = 0; i < std::max(std::thread::hardware_concurrency(), static_cast<decltype(std::thread::hardware_concurrency())>(4)); i++) {
                    pool.push_back(std::make_unique<DB>(host, user, passwd, database, port, connstr));
                    idxs.push(i);
                }
            }

            DatabasePool(std::size_t size, const char *host, const char *user, const char *passwd, const char *database, unsigned port = 0, const char *connstr = "") {
                for(std::size_t i = 0; i < size; i++) {
                    pool.push_back(std::make_unique<DB>(host, user, passwd, database, port, connstr));
                    idxs.push(i);
                }
            }

            DatabasePool(const DatabasePool&) = delete;
            DatabasePool& operator=(const DatabasePool&) = delete;
            DatabasePool(DatabasePool&&) = default;
            DatabasePool& operator=(DatabasePool&&) = default;

            std::size_t size() const noexcept { return pool.size(); }

        private:

            template<typename T>friend class DatabaseConnection;

            std::pair<std::size_t, DB*> lock() {

                // wait for an idle database
                std::unique_lock<std::mutex> guard{ mux };
                cv.wait(guard, [this](){ return !idxs.empty(); });

                // return it
                auto idx = idxs.top();
                idxs.pop();
                return { idx, pool[idx].get() };
            }

            void unlock(std::size_t idx) {
                std::lock_guard<std::mutex> guard{ mux };
                idxs.push(idx);
                cv.notify_all();
            }
    };  // class DatabasePool

    class Row {
        protected:
            int status;  ///< Status flag.

        public:

            Row(int status = 0) : status{ status } {}

            Row(const Row&) = delete;
            Row& operator=(const Row&) = delete;
            Row(Row&&) = default;
            Row& operator=(Row&&) = default;

            virtual ~Row() = default;

            virtual bool next() = 0;

            operator bool() const {
                return status == 0;
            }

            virtual bool get(unsigned column, int         &result) const = 0;
            virtual bool get(unsigned column, long        &result) const = 0;
            virtual bool get(unsigned column, double      &result) const = 0;
            virtual bool get(unsigned column, std::string &result) const = 0;

    };  // class Row

    /// Base class for all database connections.
    class Database {

        public:

            Database() = default;
            Database(const Database&) = delete;
            Database& operator=(const Database&) = delete;
            virtual ~Database() = default;

            // To fetch the first column of the result.
            virtual bool fetch(const char *statement, int         &result) = 0;
            virtual bool fetch(const char *statement, long        &result) = 0;
            virtual bool fetch(const char *statement, double      &result) = 0;
            virtual bool fetch(const char *statement, std::string &result) = 0;

            // To fetch the result as row.
            virtual std::unique_ptr<Row> fetch(const char *statement) = 0;

            /// Run a query that does not returns any result.
            virtual void query(const char*) = 0;

            /// Runs a query and returns the number of affected rows.
            virtual std::size_t query_and_check(const char*) = 0;

            /// Insert into table and return the last inserted id.
            virtual bool insert(const char*, int  &id) = 0;
            virtual bool insert(const char*, long &id) = 0;

            /// Returns the ith placeholder for statement
            virtual const char* placeholder(unsigned) const {
                return "?";
            }

            /// Escapes the given string.
            virtual std::string escape(const char*, char quote = '\'') const = 0;
            virtual std::string escape(const std::string&, char quote = '\'') const = 0;

            /// Transaction handling.
            virtual void begin()    = 0;
            virtual void commit()   = 0;
            virtual void rollback() = 0;

    };  // class Database


    /// DatabaseConnection simply delegates the interfaces of the Database
    /// class. It handles the RAII style aquisition and release of the
    /// underlying real database connection.
    template<typename DB>class DatabaseConnection {

        private:

            std::size_t      idx;            ///< The index of the database.
            DB               *db = nullptr;  ///< The real database used.

            std::function<void(std::size_t)> deleter;  ///< Function used to return the database to the pool.

        public:

            struct Transaction {};
            static constexpr Transaction transaction;

            /// Requests a new database connection from the pool.
            template<typename Pool>
            explicit DatabaseConnection(Pool &pool) : deleter{ [&pool](std::size_t idx){ pool.unlock(idx); } } {
                static_assert(std::is_base_of<DB, typename Pool::DatabaseType>::value, "Wrong database type.");
                std::tie(idx, db) = pool.lock();
            }

            /// Requests a new database connection from the pool.
            template<typename Pool>
            explicit DatabaseConnection(Pool &pool, Transaction) : deleter{ [&pool](std::size_t idx){ pool.unlock(idx); } } {
                static_assert(std::is_base_of<DB, typename Pool::DatabaseType>::value, "Wrong database type.");
                std::tie(idx, db) = pool.lock();
                begin();
            }

            DatabaseConnection(const DatabaseConnection&) = delete;
            DatabaseConnection& operator=(const DatabaseConnection&) = delete;

            #ifndef __cpp_guaranteed_copy_elision
              DatabaseConnection(DatabaseConnection &&other) : idx{ other.idx }, db{ other.db }, deleter{ other.deleter } {
                  other.deleter = 0;
              }
              DatabaseConnection& operator=(DatabaseConnection&&) = delete;
            #else
              DatabaseConnection(DatabaseConnection&&) = delete;
              DatabaseConnection& operator=(DatabaseConnection&&) = delete;
            #endif

            /// Dtor. Handles back the database (connection) to the pool.
            ~DatabaseConnection() {
                if(deleter)
                    deleter(idx);
            }

            const Database* database() const {
                return db;
            }

            template<typename T>bool fetch(const char *statement, T &result) {
                return db->fetch(statement, result);
            }

            std::unique_ptr<Row> fetch(const char *statement) {
                return db->fetch(statement);
            }

            void query(const char *statement) {
                db->query(statement);
            }

            std::size_t query_and_check(const char *statement) {
                return db->query_and_check(statement);
            }

            bool insert(const char *statement, int &id) {
                return db->insert(statement, id);
            }

            bool insert(const char *statement, long &id) {
                return db->insert(statement, id);
            }

            [[nodiscard]] const char* placeholder(unsigned ith) const {
                return db->placeholder(ith);
            }

            [[nodiscard]] std::string escape(const char *txt, char quote = '\'') const {
                return db->escape(txt, quote);
            }

            [[nodiscard]] std::string escape(const std::string &txt, char quote = '\'') const {
                return db->escape(txt, quote);
            }

            void begin() {
                db->begin();
            }

            void commit() {
                db->commit();
            }

            void rollback() {
                db->rollback();
            }

    };  // class DatabaseConnection


    template<typename DB>
    class TransactionLock {

        private:

            DB   &db;
            bool com = false;

        public:

            TransactionLock(DB &db) : db { db } { db.begin(); }

            void commit() { db.commit(); com = true; }

            ~TransactionLock() { if (!com) db.rollback(); }
    };

}  // namespace db

#endif  /* _DB_DB_H_ */
