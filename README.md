# Compile & build

* Download and install clang
  * bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
  * for more info see: https://apt.llvm.org
* Install 
  *
  *
  *
  *
  *
  *
  *
  *
* ./configure
*  make

# Run
The programs can be run from the src directory. If the NDEBUG macro is not defined, they will find the
ini file and certificates in the etc directory of the repo. With the NDEBUG macro turned on this
search path is turned off, only in the etc and /etc directories are looked up.

The names of the ini files are as follows*

|Core Element     | Name of the ini file |
------------------------------------------
| Authorizer      | authorize.ini        |
| Cert. Authority | certauthority.ini    |
| Choreographer   | choreographer.ini    |
| Dev. Registry   | devregistry.ini      |
| Event Handler   | eventhandler.ini     |
| Gatekeeper      | gatekeeper.ini       |
| Gateway         | gateway.ini          |
| Onboarding      | onboarding.ini       |
| Orchestrator    | orchestrator.ini     |
| QosMonitor      | qosmonitor.ini       |
| Serv. Registry  | serviceregistry.ini  |
| Sys. Registry   | sysregistry.ini      |

# Exit program

Ctrl + C or send a kill to it.

# Logging

By default the console is used for logging. To set a different target use the ./configure, e.g.,
```
--enable-log=syslog
```

# Certificates

## Converting p12 to separate PEM and KEY

* openssl pkcs12 -in path.p12 -out newfile.pem -clcerts -nokeys
* openssl pkcs12 -in path.p12 -out newfile.key -nocerts -nodes

## Add password to keys
* openssl rsa -aes256 -in your.key -out your.encrypted.key
