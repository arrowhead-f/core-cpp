////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      mariadb
/// Date:      2020-12-20
/// Author(s): ng201
///
/// Description:
/// * [mariadb] - Testing the interface for MariaDB.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <chrono>
#include <fstream>
#include <future>
#include <mutex>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include "../src/db/DB.h"
#include "../src/db/DBException.h"
#include "../src/db/MariaDB.h"


namespace {

    namespace settings {

        std::string host;
        unsigned port;
        std::string dbname;
        std::string user;
        std::string passwd;

    }

    bool prelude() {

        static bool ready = false;

        if (ready)
            return true;

        std::ifstream is{ "data/test0002/db.ini" };
        if (!is) 
            return false;

        std::string line;
        while(std::getline(is, line)) {
            if (line[0] == ';' || line[0] == '[' || line.empty())
                continue;

            const auto fmt = std::regex{ "([^=]+)[ ]*=[ ]*(.*)" };
            std::smatch match;
            if (!std::regex_match(line, match, fmt))
                return false;


            if (match[1].str() == "host") {
                settings::host = match[2].str();
                continue;
            }
            if (match[1].str() == "port") {
                try {
                    settings::port = stoul(match[2].str());
                }
                catch(...) {
                    return false;
                }
                continue;
            }
            if (match[1].str() == "dbname") {
                settings::dbname = match[2].str();
                continue;
            }
            if (match[1].str() == "user") {
                settings::user = match[2].str();
                continue;
            }
            if (match[1].str() == "passwd") {
                settings::passwd = match[2].str();
                continue;
            }
        }

        MYSQL mysql;

        mysql_init(&mysql);
        mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP, "arrowhead-test-prelude");

        if (!mysql_real_connect(&mysql, settings::host.c_str(), settings::user.c_str(), settings::passwd.c_str(), settings::dbname.c_str(), settings::port, nullptr, 0))
            return false;

        if (mysql_query(&mysql, "DROP TABLE IF EXISTS cats")) {
            mysql_close(&mysql);
            return false;
        }

        if (mysql_query(&mysql, "DROP TABLE IF EXISTS dogs")) {
            mysql_close(&mysql);
            return false;
        }

        if (mysql_query(&mysql, "DROP TABLE IF EXISTS mice")) {
            mysql_close(&mysql);
            return false;
        }

        ///// id              INT unsigned NOT NULL AUTO_INCREMENT, # Unique ID for the record
        ///// name            VARCHAR(150) NOT NULL,                # Name of the dog
        ///// PRIMARY KEY     (id)                                  # Make the id the primary key

        if (mysql_query(&mysql, "CREATE TABLE dogs (id INT unsigned NOT NULL PRIMARY KEY AUTO_INCREMENT, name VARCHAR(150) NOT NULL)")) {
            mysql_close(&mysql);
            return false;
        }

        if (mysql_query(&mysql, "INSERT INTO dogs (name) VALUES ('Skinny'), ('Lemy'), ('Wolfgang')")) {
            mysql_close(&mysql);
            return false;
        }

        ///// id              INT unsigned NOT NULL,                # NOT AUTO INCREMENT
        ///// name            VARCHAR(150) NOT NULL,                # Name of the mouse
        ///// PRIMARY KEY     (id)                                  # Make the id the primary key

        if (mysql_query(&mysql, "CREATE TABLE mice (id INT unsigned NOT NULL, name VARCHAR(150) NOT NULL, UNIQUE KEY unique_id (id))")) {
            mysql_close(&mysql);
            return false;
        }

        if (mysql_query(&mysql, "INSERT INTO mice (id, name) VALUES (1, 'Mickey'), (2, 'Jerry')")) {
            mysql_close(&mysql);
            return false;
        }


        ///// id              INT unsigned NOT NULL AUTO_INCREMENT, # Unique ID for the record
        ///// name            VARCHAR(150) NOT NULL,                # Name of the cat
        ///// owner           VARCHAR(150) NOT NULL,                # Owner of the cat
        ///// birth           DATE NOT NULL,                        # Birthday of the cat
        ///// state           varchar(255) NOT NULL,                # The quantum state of the cat
        ///// dog_id          bigint(20),                           # The dog that made the cat collapse
        ///// collapsed       int(1) NOT NULL DEFAULT 0,            # Whether the wave function alreadt collapsed
        ///// PRIMARY KEY     (id),                                 # Make the id the primary key
        ///// UNIQUE KEY unique_state (state)                       # The state is unique
        ///// CONSTRAINT fk_dog_id FOREIGN KEY (dog_id) REFERENCES dogs (id) ON DELETE CASCADE)

        if (mysql_query(&mysql, "CREATE TABLE cats (id INT unsigned NOT NULL PRIMARY KEY AUTO_INCREMENT, name VARCHAR(150) NOT NULL, "
                                "owner VARCHAR(150) NOT NULL, birth DATE NOT NULL, state varchar(255) NOT NULL, dog_id INT unsigned, "
                                "collapsed int(1) NOT NULL DEFAULT 0, UNIQUE KEY unique_state (state), CONSTRAINT fk_dog_id FOREIGN "
                                "KEY (dog_id) REFERENCES dogs (id) ON DELETE CASCADE)")) {
            mysql_close(&mysql);
            return false;
        }

        if (mysql_query(&mysql, "INSERT INTO cats (name, owner, birth, state, dog_id, collapsed) VALUES "
                                "  ('Sandy', 'Lennon', '2015-01-03', 'AAA', NULL, 0),                   "
                                "  ('Cookie', 'Casey', '2013-11-13', 'BBB', NULL, 0),                   "
                                "  ('Charlie', 'River', '2016-05-21', 'CCC', 1, 1);                     ")) {
            mysql_close(&mysql);
            return false;
        }

        // just to be sure
        if (mysql_query(&mysql, "SET FOREIGN_KEY_CHECKS = 1")) {
            mysql_close(&mysql);
            return false;
        }

        mysql_close(&mysql);

        return (ready = true);
    }
}


TEST_CASE("[mariadb]: Playing the prelude", "[mariadb] [db] [!mayfail]") {

    const auto res = prelude();
    if (!res) {
        WARN("Skipped because MariaDB wasn't configured.");
    }
    CHECK(res == true);

}


// -------------------------------------------------------------------------------------------------------------


class MariaDBFixture {

    protected:

        db::MariaDB *db = nullptr;

    public:

        MariaDBFixture() : db{ prelude() ? new db::MariaDB{ settings::host.c_str(), settings::user.c_str(), settings::passwd.c_str(), settings::dbname.c_str() } : nullptr }{
        }

        ~MariaDBFixture(){ delete db; }
};


TEST_CASE_METHOD(MariaDBFixture, "[mariadb]: Test fetch single value", "[mariadb] [db]") {

    if (!prelude())
        return;

    {
        int i;

        CHECK(db->fetch("SELECT collapsed FROM cats WHERE name  = 'Charlie'", i) == true);
        REQUIRE(i == 1);
    }

    {
        long l;

        CHECK(db->fetch("SELECT collapsed FROM cats WHERE name = 'Charlie'", l) == true);
        REQUIRE(l == 1);
    }

    {
        std::string s;

        CHECK(db->fetch("SELECT owner FROM cats WHERE name = 'Cookie'", s) == true);
        REQUIRE(s == "Casey");
    }

    {
        std::string s;

        CHECK(db->fetch("SELECT birth FROM cats WHERE name = 'Sandy'", s) == true);
        REQUIRE(s == "2015-01-03");
    }

}


TEST_CASE_METHOD(MariaDBFixture, "[mariadb]: Test fetch NULL value", "[mariadb] [db]") {

    if (!prelude())
        return;

    long l;
    CHECK(db->fetch("SELECT dog_id FROM cats WHERE name  = 'Sandy'", l) == false);
    CHECK(db->fetch("SELECT dog_id FROM cats WHERE name  = 'Cookie'", l) == false);
    CHECK(db->fetch("SELECT collapsed FROM cats WHERE name = 'Charlie'", l) == true);
    REQUIRE(l == 1);
}


TEST_CASE_METHOD(MariaDBFixture, "[mariadb]: Test fetch db::row", "[mariadb] [db]") {

    if (!prelude())
        return;

    {
        auto row = db->fetch("SELECT name, owner, dog_id FROM cats WHERE dog_id = 1 ORDER BY name DESC");
        REQUIRE(row);

        std::size_t count = 0;

        std::string name, owner, dog;

        do {
            std::string c2;

            CHECK(row->get(0, c2) == true);
            name += c2;

            CHECK(row->get(1, c2) == true);
            owner += c2;

            CHECK(row->get(2, c2) == true);
            dog += c2;

            count++;
        } while(row->next());


        CHECK(name  == "Charlie");
        CHECK(owner == "River");
        CHECK(dog   == "1");

        REQUIRE(count == 1);
    }

    {
        auto row = db->fetch("SELECT name, owner, dog_id FROM cats WHERE dog_id IS NULL ORDER BY name ASC");
        REQUIRE(row);

        std::size_t count = 0;

        std::string name, owner;

        do {
            std::string c2;

            CHECK(row->get(0, c2) == true);
            name += c2;

            CHECK(row->get(1, c2) == true);
            owner += c2;

            CHECK(row->get(2, c2) == false);

            count++;
        } while(row->next());


        CHECK(name  == "CookieSandy");
        CHECK(owner == "CaseyLennon");

        REQUIRE(count == 2);
    }

}


TEST_CASE_METHOD(MariaDBFixture, "[mariadb]: Test syntax error in SQL query", "[mariadb] [db]") {

    if (!prelude())
        return;

    long l;
    CHECK_THROWS(db->fetch("SELECT dog_id FROM cats WHERE name = 'Sandy' ORDER name ASC", l));
    CHECK_THROWS(db->fetch("SELECT phase  FROM cats WHERE name = 'Sandy'", l));
}


TEST_CASE_METHOD(MariaDBFixture, "[mariadb]: Test query", "[mariadb] [db]") {

    if (!prelude())
        return;

    // test update
    CHECK_NOTHROW(db->query("UPDATE dogs SET name = 'Lemmy' WHERE name = 'Lemy'"));

    int id;
    REQUIRE(db->fetch("SELECT id FROM dogs WHERE name = 'Lemy'", id)  == false);  // it was changed with the
    REQUIRE(db->fetch("SELECT id FROM dogs WHERE name = 'Lemmy'", id) == true);   // update query right before

    // test delete
    int cnt1, cnt2;
    CHECK(db->fetch("SELECT COUNT(*) FROM mice", cnt1)  == true);
    CHECK_NOTHROW(db->query("DELETE FROM mice WHERE name = 'Mickey'"));
    CHECK(db->fetch("SELECT COUNT(*) FROM mice", cnt2)  == true);

    REQUIRE(cnt1 == cnt2 + 1);
}


TEST_CASE_METHOD(MariaDBFixture, "[mariadb]: Test insert/returning", "[mariadb] [db]") {

    if (!prelude())
        return;

    int id;
    REQUIRE(db->insert("INSERT INTO dogs (name) VALUES ('Spion')", id) == true);          // should be okay
    REQUIRE(id > 0);

    REQUIRE(db->insert("INSERT INTO mice (id, name) VALUES (3, 'Dinky')", id) == false);  // id is not an auto increment column
    CHECK_THROWS(db->insert("INSERT INTO mice (name) VALUES ('Snuffles')", id));          // id is not an auto increment column
}


TEST_CASE_METHOD(MariaDBFixture, "[mariadb]: Test breaking a constraint", "[mariadb] [db]") {

    using namespace std::literals;

    if (!prelude())
        return;

    long id = 0, nid = 0;
    REQUIRE(db->fetch("SELECT id + 1 AS next_id FROM dogs ORDER BY id DESC LIMIT 1", id) == true);  // get an unused id

    // foreign key check fails
    CHECK_THROWS(db->query(("UPDATE cats SET dog_id = "s + std::to_string(id) + " WHERE dog_id IS NULL").c_str()));

    // foreign key check fails
    CHECK_THROWS(db->insert(("INSERT INTO cats (name, owner, birth, state, dog_id) VALUES ('Shroedinger', 'Schroedinger', '1935-06-08', 'QQQ'," + std::to_string(id) +  ")").c_str(), nid));

    // unique key check fails
    CHECK_THROWS(db->insert("INSERT INTO cats (name, owner, birth, state) VALUES ('Shroedinger', 'Schroedinger', '1935-06-08', 'AAA')", nid));
}


// -------------------------------------------------------------------------------------------------------------


TEST_CASE("[mariadb]: Test SELECT with db from pool", "[mariadb] [db]") {

    if (!prelude())
        return;

    db::DatabasePool<db::MariaDB> pool{ settings::host.c_str(), settings::user.c_str(), settings::passwd.c_str(), settings::dbname.c_str() };
    auto db = db::DatabaseConnection<db::DatabasePool<db::MariaDB>::DatabaseType>{ pool };

    long l;
    CHECK(db.fetch("SELECT COUNT(*) FROM dogs", l) == true);
    REQUIRE(l > 3);  // there should be at least 3 dogs

}


TEST_CASE("[mariadb]: Test that multiple parallel connections are okay", "[mariadb] [db]") {

    if (!prelude())
        return;

    db::DatabasePool<db::MariaDB> pool{ settings::host.c_str(), settings::user.c_str(), settings::passwd.c_str(), settings::dbname.c_str() };

    // the number of available connections
    const auto num = pool.size();

    // get how much dogs we have
    long dogs = 0;
    {
        auto db = db::DatabaseConnection<db::DatabasePool<db::MariaDB>::DatabaseType>{ pool };
        CHECK(db.fetch("SELECT COUNT(*) FROM dogs", dogs) == true);
    }

    std::vector<std::thread> v;
    std::size_t cnt = 0, max = 0;
    std::mutex mux;

    // run many db queries, more than the available connections
    for(std::size_t i = 0; i < 3 * num; i++) {
        v.emplace_back(
            [&max, &cnt, &mux, dogs, &pool] {

                auto db = db::DatabaseConnection<db::DatabasePool<db::MariaDB>::DatabaseType>{ pool };

                // count the max parallel connections
                {
                    std::scoped_lock<std::mutex> _{ mux };
                    cnt++;
                    if (cnt > max) max = cnt;
                }

                std::this_thread::yield();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                // do some query
                long d;
                CHECK(db.fetch("SELECT COUNT(*) FROM dogs", d) == true);
                CHECK(d == dogs);

                std::this_thread::yield();
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                {
                    std::scoped_lock<std::mutex> _{ mux };
                    cnt--;
                }
            }
        );
    }

    // join threads
    for(auto &th : v){
        th.join();
    }

    // check the number of parallel connections
    REQUIRE(max <= num);
}


TEST_CASE("[mariadb]: Test that parallel escapings do not ineterfere", "[mariadb] [db]") {

    if (!prelude())
        return;

    db::DatabasePool<db::MariaDB> pool{ settings::host.c_str(), settings::user.c_str(), settings::passwd.c_str(), settings::dbname.c_str() };

    // the number of available connections
    const auto num = pool.size();

    std::vector<std::thread> v;

    // run many db queries, more than the available connections
    for(std::size_t i = 0; i < 3 * num; i++) {
        v.emplace_back(
            [&pool, id = i] {

                auto db = db::DatabaseConnection<db::DatabasePool<db::MariaDB>::DatabaseType>{ pool };

                // differenct sizes to check the buffers
                switch(id % 3) {
                    case 1:
                                CHECK(db.escape("pop'py")  == R"(pop\'py)");
                                CHECK(db.escape("popp''y") == R"(popp\'\'y)");
                                CHECK(db.escape(std::string(512, 'B').c_str()) == std::string(512, 'B'));
                                CHECK(db.escape("''poppy") == R"(\'\'poppy)");
                                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                                CHECK(db.escape("poppy")   == R"(poppy)");
                    case 2:
                                CHECK(db.escape(std::string(256, 'A').c_str()) == std::string(256, 'A'));
                                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                                CHECK(db.escape("dandelion'")  == R"(dandelion\')");
                                CHECK(db.escape("dande'liondandeliondandeliondandeliondandeliondandelion") == R"(dande\'liondandeliondandeliondandeliondandeliondandelion)");
                                CHECK(db.escape("dandelion''") == R"(dandelion\'\')");
                                CHECK(db.escape("dandelion")   == R"(dandelion)");
                    default:
                                CHECK(db.escape("d'aisy")  == R"(d\'aisy)");
                                CHECK(db.escape("d''aisy") == R"(d\'\'aisy)");
                                CHECK(db.escape("'daisy'") == R"(\'daisy\')");
                                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                                CHECK(db.escape(std::string(128, 'C').c_str()) == std::string(128, 'C'));
                                CHECK(db.escape("daisy")   == R"(daisy)");
                }
            }
        );
    }

    // join threads
    for(auto &th : v){
        th.join();
    }

}
