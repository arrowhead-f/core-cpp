#include <cstring>
#include <iostream>

#include "db/DB.h"
#include "db/MariaDB.h"

#include "modules/ServiceRegistry/ServiceRegistry.hpp"

int main(int argc, char *argv[]) {

    if(!std::strcmp(argv[0], "TestModule")) {
        std::cout << "~~~ TestModule ~~~\n";
        return 0;
    }

    // create a pool of database connection
    db::DatabasePool<db::MariaDB> pool{ "127.0.0.1", "service_registry", "ZzNNpxrbZGVvfJ8", "arrowhead" };
    printf("DB connected successfully to 127.0.0.1 (service_registry, arrowhead)\n");

    // create ServiceRegistry
    ServiceRegistry oServiceRegistry;

    if(oServiceRegistry.startHTTPServer(16223, false, &pool))
    {
        printf("ServiceRegistry started successfully on port 16223!\n");
        while(1)
            sleep(1000);
    }
    else
        printf("Error: Could not start ServiceRegistry!\n");

    return 0;
}
