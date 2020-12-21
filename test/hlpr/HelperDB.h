#ifndef _TEST_HELPERDB_H_
#define _TEST_HELPERDB_H_


#include <initializer_list>
#include <string>
#include <list>
#include <set>

#include "MockDBase.h"

#include "../../src/db/DBException.h"


class HelperDB : public MockDBase {

    private:

        struct DecisionVector {

            std::size_t order     = 0;        // how deep the rabbit hole goes
            std::set<std::size_t> decision;   // the exception position

            bool iscopied         = false;
            std::size_t cnt       = 0;        // if decision vector is the same as one with
                                      // oder - 1, here we store the counts for that one

            DecisionVector() = default;
            DecisionVector(std::size_t order, std::initializer_list<std::size_t> d)
                : order{ order }
                , decision{ d }
            {}

            bool hit(std::size_t ith) const {
                return (decision.find(ith) != decision.end());
            }
        };

        std::size_t counter = 0;   ///< The counter for the exception testing
        DecisionVector dv;         ///< The decision vector used during exception testing

        sqlite3 *backup;           ///< The backup sqlite database

    public:

        using MockDBase::MockDBase;

        bool fetch(const char *statement, int &result) override {
            if(dv.hit(counter++))
                throw db::Exception{ 4, "Statement fetch error.3333" };

            return MockDBase::fetch(statement, result);
        }

        bool fetch(const char* statement, long &result) override {
            if(dv.hit(counter++))
                throw db::Exception{ 4, "Statement fetch error.3333" };

            return MockDBase::fetch(statement, result);
        }

        bool fetch(const char *statement, double &result) override {
            if(dv.hit(counter++))
                throw db::Exception{ 4, "Statement fetch error.3333" };

            return MockDBase::fetch(statement, result);
        }

        bool fetch(const char *statement, std::string &result) override {
            if(dv.hit(counter++))
                throw db::Exception{ 4, "Statement fetch error.3333" };

            return MockDBase::fetch(statement, result);
        }

        std::unique_ptr<db::Row> fetch(const char *statement) override {
            if(dv.hit(counter++))
                throw db::Exception{ 4, "Statement fetch error.3333" };

            return MockDBase::fetch(statement);
        }

        void query(const char *statement) override {
            if(dv.hit(counter++))
                throw db::Exception{ 4, "Statement fetch error.3333" };

            MockDBase::query(statement);
        }

    private:

        bool bstore() {
            int rc = sqlite3_open(":memory:", &backup);
            if (rc) {
                sqlite3_close(backup);
                backup = nullptr;
                return false;
            }

            sqlite3_backup *pBackup = sqlite3_backup_init(backup, "main", db, "main");
            if (!pBackup) {
                return false;
            }

            sqlite3_backup_step(pBackup, -1);
            sqlite3_backup_finish(pBackup);

            if (auto rc = sqlite3_errcode(db)) {
                return false;
            }

            return true;
        }

        bool breset() {

            sqlite3_backup *pBackup = sqlite3_backup_init(db, "main", backup, "main");
            if (!pBackup) {
                return false;
            }

            sqlite3_backup_step(pBackup, -1);
            sqlite3_backup_finish(pBackup);

            if (auto rc = sqlite3_errcode(db)) {
                return false;
            }

            return true;
        }

        void bdestroy() {
            if(backup)
                sqlite3_close(backup);
            backup = nullptr;
        }

    public:


        template<typename F>std::size_t run_except(F f) {

            bstore();

            std::size_t exnum = 0;

            std::list<DecisionVector> pool;

            pool.push_back(DecisionVector{ 0, {} });
            pool.push_back(DecisionVector{ 0, {0} });

            while (!pool.empty()) {

                if(!breset())
                    std::cerr << "Backup ERROR\n";

                dv = pool.front();
                pool.pop_front();
                counter = 0;

                if (!dv.iscopied) {
                    try {
                        f();
                    }
                    catch (...) {
                        exnum++;
                        continue;
                    }

                    dv.cnt = counter;
                }

                if (dv.order + 1 == dv.cnt)
                    continue;

                {
                    DecisionVector dOff { dv.order + 1, {}};
                    dOff.decision.insert(std::begin(dv.decision), std::end(dv.decision));
 
                    dOff.iscopied = true;
                    dOff.cnt = dv.cnt;
                    pool.push_back(std::move(dOff));
                }

                {
                    DecisionVector dON { dv.order + 1, {}};
                    dON.decision.insert(std::begin(dv.decision), std::end(dv.decision));
                    dON.decision.insert(dv.order + 1);

                    pool.push_back(std::move(dON));
                }

            }

            bdestroy();
            return exnum;
        }


};  // class HelperDB


#endif  /* _TEST_HELPERDB_H_ */
