////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      dbpool
/// Date:      2020-12-20
/// Author(s): ng201
///
/// Description:
/// * [dbpool] - Testing the DBPool.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <atomic>
#include <chrono>
#include <future>
#include <mutex>
#include <thread>
#include <vector>

#include "../src/db/DB.h"
#include "../src/db/DBException.h"

#include "hlpr/MockDBase.h"


TEST_CASE("[dbpool]: Test the size of the pool", "[dbpool] [db] [!mayfail]") {

    db::DatabasePool<MockDBase> pool{ "who-cares", "this", "is-a", "mock-db" };

    // the number of available connections to the db
    const auto num = pool.size();

    REQUIRE(num > 1);
}


TEST_CASE("[dbpool]: Test that the number of max db connections cannot be exceeded", "[dbpool] [db]") {

    db::DatabasePool<MockDBase> pool{ "who-cares", "this", "is-a", "mock-db" };

    // the number of available connections
    const auto num = pool.size();

    if (num <= 1)
        return;

    std::vector<std::thread> v;
    std::size_t cnt = 0, max = 0;
    std::mutex mux;

    // run many db queries, more than the available connections
    for(std::size_t i = 0; i < 3 * num; i++) {
        v.emplace_back(
            [&max, &cnt, &mux, &pool] {
                auto db = db::DatabaseConnection<db::DatabasePool<MockDBase>::DatabaseType>{ pool };

                // count the max parallel connections
                {
                    std::scoped_lock<std::mutex> _{ mux };
                    cnt++;
                    if (cnt > max) max = cnt;
                }

                std::this_thread::yield();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));

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


TEST_CASE("[dbpool]: Test the spawning of dependent threads with db connections", "[dbpool] [db]") {

    ///// the dependency of threads during this test
    /////
    ///// a -----> b means b can start after a
    /////
    /////
    ///// [0] ---> [1] ---> ... ---> [n-1]    } before the use action all of them should be started
    /////  ^        ^
    /////  |        |
    /////  |        |                         } before the user's action an extra thread is started
    ///// user     [n]                        } that should block, because there's no empty space
    /////                                     } in the pool
    /////

    db::DatabasePool<MockDBase> pool{ "who-cares", "this", "is-a", "mock-db" };

    // the number of available connections
    const auto num = pool.size();

    if (num <= 1)
        return;

    std::vector<std::thread> v;
    std::vector<std::promise<void>> prom;
    std::vector<std::future<void>> fut;

    for(std::size_t i = 0; i < num + 1; i++) {
        std::promise<void> p;
        auto f = p.get_future();

        prom.push_back(std::move(p));
        fut.push_back(std::move(f));
    }

    // to count all thread are at POINT.2
    std::atomic<std::size_t> there = 0;
    std::atomic<bool> flag = false;

    // run many db queries, more than the available connections
    for(std::size_t i = 0; i < num; i++) {
        v.emplace_back(
            [&prom, &fut, &pool, id = i, &there, &flag] {
                auto db = db::DatabaseConnection<db::DatabasePool<MockDBase>::DatabaseType>{ pool };

                // POINT.1
                there++;
                fut[id].wait();

                // one of them is waiting this flag to be set;
                // this flag will be set by an extra thread, thus
                // here it is important to use diferent id than 0
                if (id == 1) {
                    while(!flag.load()) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(20));
                    }
                }

                // POINT.2
                prom[id + 1].set_value();
            }
        );
    }

    // wait for every thread reach POINT.1
    while(there.load() != num) {
        std::this_thread::sleep_for(std::chrono::milliseconds(6));
    }

    std::promise<void> point1, point2;
    std::future<void> f1 = point1.get_future(), f2 = point2.get_future();

    // add an extra thread; there cannot be any free db connections
    v.emplace_back(
        [&prom, &pool, &point1, &point2, &flag] {

            point1.set_value();

            {
                auto db = db::DatabaseConnection<db::DatabasePool<MockDBase>::DatabaseType>{ pool };

                point2.set_value();
                flag = true;
            }

        }
    );

    // should wait at point1; this one is reachable
    f1.wait();

    // still cannot be at point2; wait for it a bit
    REQUIRE(f2.valid() == true);
    auto status = f2.wait_for(std::chrono::milliseconds(100));
    REQUIRE(status == std::future_status::timeout);

    // start the processing of the threads
    prom[0].set_value();

    // wait for all to finish
    fut[num].wait();

    // the extra thread reached point2
    f2.wait();

    // join all the threads
    for(auto &th : v){
        th.join();
    }
}
