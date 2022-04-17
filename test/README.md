# Tests

### Test areas

* [0001, 0015] - db tests
* [0075, 0099] - http server and client tests
* [0100, 0499] - core element tests
* [0500, 0999] - server tests
* [1000, 1099] - selftests

### List of test

| name | tags                        | description                     |
|------|-----------------------------|---------------------------------|
| 0001 | dbpool, db                  | check the pool of databases     |
| 0001 | mariadb, db                 | check the MariaDB wrapper       |
| 0051 | inifile                     | test the option parser          |
| 0051 | inifile                     | test the ini file reader/parser |
| 0052 | KeyProvider                 | check the parsing of the files  |
| 0053 | inifile                     | test the ini file reader/parser |
| 0054 | jsonwriter                  | check json string creation      |
| 0055 | crypto, sha256              | checking sha256 generation      |
| 0056 | jsonwriter                  | checks jsonwriter with db::Row  |
| 0057 | jsonview                    | checks JsonView and JsonCView   |
| 0058 | urlparser                   | checks the url parser class     |
| 0059 | x509cert                    | checks the x509cert class       |
| 0093 | rbcurl, client              | test Curl-based request builder |
| 0100 | core, core_utils            | test core utility functions     |
| 0102 | core, cert_authority        | test CertAuthority, happy path  |
| 0103 | core, cert_authority        | test CertAuthority, sad path    |
| 0104 | core, cert_authority, query | test CAQueries                  |
| 0105 | core, service_registry      | test ServiceRegistry            |
| 0106 | core, sservice_registry     | test ServiceRegistry            |
| 0108 | core, orchestrator          | test Orchestrator               |
| 0109 | core, orchestrator          | test Orchestrator               |
| 0110 | core, orchestrator, query   | test queries for Orchestrator   |
| 0111 | core, gatekeeper            | test Gatekeeper (happy path)    |
| 0112 | core, gatekeeper            | test Gatekeeper (sad path)      |
| 0113 | core, gatekeeper, query     | test queries for Gatekeeper     |
| 0114 | core, choreographer         | test Choreographer (happy path) |
| 0115 | core, choreographer         | test Choreographer (sad path)   |
| 0116 | core, choreographer, query  | test queries for Choreographer  |
| 0117 | core, dev_registry          | test DevRegistry (happy path)   |
| 0118 | core, dev_registry          | test DevRegistry (sad path)     |
| 0119 | core, dev_registry, query   | test queries for DevRegistry    |
| 0120 | core, eventhandler          | test EventHandler (happy path)  |
| 0121 | core, eventhandler          | test EventHandler (sad path)    |
| 0122 | core, eventhandler, query   | test queries for EventHandler   |
| 0123 | core, gateway               | test Gateway (happy path)       |
| 0124 | core, gateway               | test Gateway (sad path)         |
| 0125 | core, gateway, query        | test queries for Gateway        |
| 0126 | core, onboarding            | test Onboarding (happy path)    |
| 0127 | core, onboarding            | test Onboarding (sad path)      |
| 0128 | core, onboarding, query     | test queries for Onboarding     |
| 0129 | core, qos_monitor           | test QosMonitor (happy path)    |
| 0130 | core, qos_monitor           | test QosMonitor (sad path)      |
| 0131 | core, qos_monitor, query    | test queries for QosMonitor     |
| 0132 | core, sys_registry          | test SysRegistry (happy path)   |
| 0133 | core, sys_registry          | test SysRegistry (sad path)     |
| 0134 | core, sys_registry, query   | test queries for SysRegistry    |
| 0135 | core, authorization         | test Authorization (happy path) |
| 0136 | core, authorization         | test Authorization (sad path)   |
| 0137 | core, authorization, query  | test queries for Authorization  |
| 0501 | server, request_parser      | test request parser             |
| 0502 | server, https               | test the https server           |
| 0502 | server, https               | test the https server           |
| 1000 | selftest, mockcurl          | test the MockCurl class         |
| 1001 | selftest, mockdbase         | test the MockDBase class        |
| 1002 | selftest, jsoncomp          | test json object comparison     |
| 1003 | selftest, helperdb          | test the HelperDB class         |
| 1004 | selftest, mochdbase, sql    | test MockDBase sql capabilities |

### Detailed description

* 0001
  * tags: [dbpool] [db]
  * description: tests the DBPool class
* 0002
  * tags: [mariadb] [db]
  * description: tests the MariaDB class; tests the real database if connection data is provided in the data directory
* 0052
  * tags: [KeyProvider]
  * description
* 0053
  * tags: [parsers]
  * description
* 0054
  * tags: [jsonwriter]
  * description
* 0055
  * tags: [crypto] [sha256]
  * description
* 0056
  * tags: [jsonwriter]
  * description: test JsonWriter with db::Row
* 0093
  * tags: [rbcurl] [client]
  * description: tests the Curl-based request builder
* 1000
  * tags: [selftest] [mockcurl]
  * description: test the MockCurl class
* 1001
  * tags: [selftest] [mockdbase]
  * description: test the MockDBase class
* 1002
  * tags: [selftest] [jsoncomp]
  * description: test json object comparison
* 1003
  * tags: [selftest] [helperdb]
  * description: test exception tester for mock databases
* 1004
  * tags: [selftest] [mockdbase] [sql]
  * description: test sql capabilities of MockDBase
