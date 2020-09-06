////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      ???
/// Date:      2020-09-01
/// Author(s): ng201
///
/// Description:
/// * [parsers] - This will be some kind of config reader
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>

#include "core/ServiceRegistry/ServiceRegistry.h"


/* the database adapter */
#include "db/DB.h"
#include "db/MariaDB.h"

/* core system elements */
#include "core/ServiceRegistry/ServiceRegistry.h"

/* connections*/
#include "net/KeyProvider.h"
#include "net/ReqBuilder.h"

#include "utils/traits.h"

TEST_CASE("ServiceRegistry: chek register", "[ServiceRegistry]") {

    db::DatabasePool<db::MariaDB> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    auto kp = KeyProvider{ "data/test0052/tempsensor.txt", "AAA", "data/test0052/tempsensor.txt", "BBB", "12345", "data/test0052/tempsensor.txt" };
    ReqBuilder reqBuilder{ kp };

    // create core system element
    CoreElement<CoreElementType::ServiceRegistry, db::DatabasePool<db::MariaDB>>::Type coreElement { pool, reqBuilder };

    std::cout << coreElement.processRegister(R"(
    { 
  "serviceDefinition": "TestService10",
  "providerSystem": {
    "systemName": "TestProviderSystemName10",
    "address": "10.1.2.3",
    "port": 2345,
    "authenticationInfo": "authInfo"
  },
  "serviceUri": "/akarmi/url10",
  "endOfValidity": "2022-08-05 12:00:00",
  "secure": "NOT_SECURE",
  "metadata": {
    "additionalProp1": "meta1",
    "additionalProp2": "meta2",
    "additionalProp3": "meta3"
  },
  "version": 2,
  "interfaces": [
    "interface1"
  ]
})") << "\n";

}
