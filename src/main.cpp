#include <cstring>
#include <iostream>

#include "utils/pgetopt.h"
#include "utils/traits.h"

#include "db/DB.h"
#include "db/MariaDB.h"

// delete them
class DummyRegistry {
    public:
        void print() const {
            std::cout << "DummyRegistry\n";
        }
};

class ServiceRegistry {
    public:
        void print() const {
            std::cout << "ServiceRegistry\n";
        }
};


int main(int argc, char *argv[]) {

    //if(!std::strcmp(argv[0], "TestModule")) {
    //    std::cout << "~~~ TestModule ~~~\n";
    //    return 0;
    //}

    // create a pool of database connection
    db::DatabasePool<db::MariaDB> pool{ "127.0.0.1", "root", "root", "capi" };

    Module<ModuleType::MODULE>::Type x { /* add the pool, store by reference */ };

    x.print();

    {
        // first we need to get a database from the pool
        // until this db is present, we can use it...
        auto db = db::DatabaseConnection<db::DatabasePool<db::MariaDB>::DatabaseType>{ pool };

        std::string result;
        auto rc = db.fetch("SELECT first_name, status, priority FROM employees;", result);

        if (!rc) {
            std::cout << "Null value read.\n";
        }

        std::cout << "The returned value is: " << result << "\n";

        //db.query("INSERT INTO employees (first_name, last_name, status, priority) VALUES ('Alma', 'Korte', 3, 110)");

        if(auto row = db.fetch("SELECT first_name, status, priority FROM employees;")) {

            do{

                std::string name;
                int status;

                row->get(0, name);
                row->get(1, status);

                std::cout << "status: " << status << ", first_name: " << name << "\n";

            } while (row->next());
        }
    }

    return 0;
}
