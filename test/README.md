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
| 0056 | jsonwriter                  | check jsonwriter with db::Row   |
| 0093 | rbcurl, client              | test Curl-based request builder |
| 0102 | core, cert_authority        | test CertAuthority, happy path  |
| 0103 | core, cert_authority        | test CertAuthority, sad path    |
| 0104 | core, cert_authority, query | test CAQueries                  |
| 0501 | server, request_parser      | test request parser             |
| 0502 | server, httpsserver         | test the https server           |
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
