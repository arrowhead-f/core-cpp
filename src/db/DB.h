#ifndef _DB_H_
#define _DB_H_

#include <stack>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <iostream>
#include <functional>
#include <type_traits>

#include "DBException.h"

namespace db {

    class Database;

    class UniqueDatabaseConnectionAllocator {};
    class MultiDatabaseConnectionAllocator {};

    template<typename DB>class DatabaseConnectionAllocator : public MultiDatabaseConnectionAllocator{
        private:
        public:

            template<typename ...Args>static std::unique_ptr<DB> create(Args&& ...args) {
                return std::make_unique<DB>(std::forward<Args>(args)...);
            }

            static std::size_t limit() {
               return 3;
            }
    };

    /// The database pool.
    template<typename DB = Database, typename DBAllocator = DatabaseConnectionAllocator<DB>
            , typename X = std::void_t<
                    typename std::enable_if<
                            std::is_base_of<MultiDatabaseConnectionAllocator, DBAllocator>::value
                    >::type
               >
           >
    class DatabasePool {
        private:

            std::mutex mux;              ///< Mutex to access the shared state.
            std::condition_variable cv;  ///<

            std::stack<std::size_t>          idxs;   ///< The stack of indexes.
            std::vector<std::unique_ptr<DB>> pool;   ///< The pool of database connections.

        public:

            using DatabaseType = DB;

            DatabasePool(const char *host, const char *user, const char *passwd, const char *database, const char *connstr = "") {
                for(std::size_t i = 0; i < DBAllocator::limit(); i++) {
                    pool.push_back(DBAllocator::create(host, user, passwd, database));
                    idxs.push(0);
                }
            }

            DatabasePool(const DatabasePool&) = delete;
            DatabasePool& operator=(const DatabasePool&) = delete;
            DatabasePool(DatabasePool&&) = default;
            DatabasePool& operator=(DatabasePool&&) = default;

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

    class Statement;

    class PreparedStatementImpl {
        public:

            virtual void reset() = 0 ;

            virtual void bind_next(int) = 0;
            virtual void bind_next(long) = 0;
            virtual void bind_next(double) = 0;
            virtual void bind_next(const std::string&) = 0;

            virtual Row* execute() = 0;

            virtual ~PreparedStatementImpl() = default;
    };

    class PreparedStatement {
        private:
            std::unique_ptr<PreparedStatementImpl> stmt;
            std::unique_ptr<Row>                   row = nullptr;
        public:

            PreparedStatement(std::unique_ptr<PreparedStatementImpl> &&stmt) : stmt{ std::move(stmt) } { }

            void reset() {
                stmt->reset();
                row.release();
            }

            template<typename T>PreparedStatement& operator<<(T &&val) {
                stmt->bind_next(std::forward<T>(val));
                return *this;
            }

            void prepare() {
                row.reset(stmt->execute());
            }

            class iterator {
                private:
                    const PreparedStatement &stmt;
                    Row                     *row;
                public:

                    iterator(const PreparedStatement &stmt, Row *row) : stmt{ stmt }, row{ row } { }

                    iterator operator++() {
                        row->next();
                        return *this;
                    }

                    bool operator==(const iterator &it) { return row == it.row || (row->operator bool() == false && it.row == nullptr); }
                    bool operator!=(const iterator &it) { return row != it.row && (row->operator bool()          && it.row == nullptr); }

                    // disallow to call next on the returned row directly
                    const Row& operator*()  { return *row; }
                    const Row* operator->() { return row;  }
            };

            iterator begin() { return {*this, !row ? (prepare(), row.get()) : nullptr }; }
            iterator end()   { return {*this, nullptr};   }

    };

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

            /// Returns the ith placeholder for statement
            virtual std::string placeholder(unsigned) const {
                return "?";
            }

            /// Escapes the given string.
            virtual std::string escape(const char*, char quote = '\'') const = 0;

            //virtual PreparedStatement prepare(std::size_t) = 0;
            virtual PreparedStatement prepare(const Statement &stmt, std::size_t id) = 0;

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

            /// Requests a new database connection from the pool.
            template<typename T = Database, typename DBAllocator = DatabaseConnectionAllocator<T>>
            explicit DatabaseConnection(DatabasePool<T, DBAllocator> &pool) : deleter{ [&pool](std::size_t idx){ pool.unlock(idx); } } {
                static_assert(std::is_base_of<DB, T>::value, "Wrong database type.");
                std::tie(idx, db) = pool.lock();
            }

            /// Dtor. Handles back the database (connection) to the pool.
            ~DatabaseConnection() {
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

            [[nodiscard]] std::string placeholder(unsigned ith) const {
                return db->placeholder(ith);
            }

            [[nodiscard]] std::string escape(const char *txt, char quote = '\'') const {
                return db->escape(txt, quote);
            }

            [[nodiscard]] PreparedStatement prepare(const Statement &stmt, std::size_t id = 0) {
                return db->prepare(stmt, id);
            }

    };  // class DatabaseConnection

    // placeholder for the statements
    struct placeholder_t {};                        ///< Placeholder type for the statements.
    static constexpr placeholder_t placeholder {};  ///< Placeholder to be used with the statements.

    // statement modifiers to escape text in statements
    struct escape_t {};                              ///< Escape type for the statements.
    static constexpr escape_t escape {};             ///< Used to escape string values.
    struct escape_table_t {};                        ///< Escape type to escape table names.
    static constexpr escape_table_t escape_tbl {};   ///<
    struct escape_column_t {};                       ///< Escape type to escape column names.
    static constexpr escape_column_t escape_col {};  ///<

    class Statement {
        private:
            const Database *db;        ///< The underlying database.

            std::ostringstream query;  ///< The query built.
            unsigned ith = 0;          ///< The current parameters number.

            enum EscapeState { NOTHING = 0, TABLE = '`', COLUMN = '"', PARAM = '\'' };
            EscapeState state = NOTHING;         ///< Whether to do escaping.

            std::string escape_impl(const char*, char quote);

        public:

            template<typename DB>
            explicit Statement(const DatabaseConnection<DB> &db) : db{ db.database() } {}

            Statement& operator<<(const char *txt) {
                if(state) {
                    query << ' ' << static_cast<char>(state) << escape_impl(txt, state) << static_cast<char>(state) << ' ';
                    state = NOTHING;
                }
                else {
                    query << ' ' << txt << ' ';
                }
                return *this;
            }

            Statement& operator<<(const std::string &txt) {
                return operator<<(txt.c_str());
            }

            template<typename T>auto operator<<(T &&val) -> typename std::enable_if<!std::is_convertible<typename std::remove_reference<T>::type, std::string>::value, Statement&>::type{
                query << ' ' << std::forward<T>(val) << ' ';
                return *this;
            }

            /// Print a placeholder into the statement.
            Statement& operator<<(placeholder_t);

            // To alter the state of the Statement.
            Statement& operator<<(escape_table_t)  { state = TABLE;  return *this; }
            Statement& operator<<(escape_column_t) { state = COLUMN; return *this; }
            Statement& operator<<(escape_t)        { state = PARAM;  return *this; }

            inline friend std::ostream& operator<<(std::ostream &out, const Statement &stmt) {
                out << stmt.query.str();
                return out;
            }

            [[nodiscard]] std::string str() const {
                return query.str();
            }

            #if __cplusplus >= 202002L
                [[nodiscard]] std::string_view view() const {
                    return query.view();
                }
            #endif

            void pretty_print(std::ostream &out) const {
                // pretty print the SQL
            }


    };  // class Statement

    inline Statement& Statement::operator<<(placeholder_t) {
        query << db->placeholder(ith);
        return *this;
    }

    inline std::string Statement::escape_impl(const char *val, char quote) {
        return db->escape(val, quote);
    }

}  // namespace db

#endif  /* _DB_H_ */
