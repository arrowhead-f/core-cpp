bin_PROGRAMS = authorization    \
               cert_authority   \
               choreographer    \
               dev_registry     \
               eventhandler     \
               gatekeeper       \
               gateway          \
               onboarding       \
               orchestrator     \
               qos_monitor      \
               service_registry \
               sys_registry

noinst_LIBRARIES = libgason.a libjson11.a libutils.a libcrate.a
libgason_a_SOURCES = ../inc/gason/gason.cpp
libjson11_a_SOURCES = ../inc/json11/json11.cpp
libutils_a_SOURCES = utils/pgetopt.cpp utils/inifile.cpp

libcrate_a_SOURCES = http/crate/Response.cpp     \
                     http/crate/Uri.cpp          \
                     http/crate/X509Cert.cpp     \
                     http/serv/RequestParser.cpp


AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/inc -I$(top_srcdir)/src -include boot/features.h
AM_LDFLAGS = $(shell mariadb_config --libs)

# libs handled automatically by the configure script: -lpthread -lssl -lcrypto -lcurl

sources = main.cpp                         \
          boot/boot.cpp                    \
          http/wget/WG_Curl.cpp

libraries = libgason.a libjson11.a libutils.a libcrate.a

#sources += http/crate/Response.cpp         \
#           http/crate/X509Cert.cpp         \
#           http/serv/RequestParser.cpp

################################################################################################
################################################################################################
################################################################################################

authorization_CPPFLAGS = -include boot/traits/Authorization.h $(AM_CPPFLAGS)
authorization_SOURCES = $(sources)
authorization_LDADD = $(libraries)

cert_authority_CPPFLAGS = -include boot/traits/CertAuthority.h $(AM_CPPFLAGS)
cert_authority_SOURCES = $(sources) 
cert_authority_LDADD = $(libraries)

choreographer_CPPFLAGS = -include boot/traits/Choreographer.h $(AM_CPPFLAGS)
choreographer_SOURCES = $(sources)
choreographer_LDADD = $(libraries)

dev_registry_CPPFLAGS = -include boot/traits/DevRegistry.h $(AM_CPPFLAGS)
dev_registry_SOURCES = $(sources)
dev_registry_LDADD = $(libraries)

eventhandler_CPPFLAGS = -include boot/traits/EventHandler.h $(AM_CPPFLAGS)
eventhandler_SOURCES = $(sources)
eventhandler_LDADD = $(libraries)

gatekeeper_CPPFLAGS = -include boot/traits/Gatekeeper.h $(AM_CPPFLAGS)
gatekeeper_SOURCES = $(sources)
gatekeeper_LDADD = $(libraries)

gateway_CPPFLAGS = -include boot/traits/Gateway.h $(AM_CPPFLAGS)
gateway_SOURCES = $(sources)
gateway_LDADD = $(libraries)

onboarding_CPPFLAGS = -include boot/traits/Onboarding.h $(AM_CPPFLAGS)
onboarding_SOURCES = $(sources) 
onboarding_LDADD = $(libraries)

orchestrator_CPPFLAGS = -include boot/traits/Orchestrator.h $(AM_CPPFLAGS)
orchestrator_SOURCES = $(sources) 
orchestrator_LDADD = $(libraries)

qos_monitor_CPPFLAGS = -include boot/traits/QosMonitor.h $(AM_CPPFLAGS)
qos_monitor_SOURCES = $(sources)
qos_monitor_LDADD = $(libraries)

service_registry_CPPFLAGS = -include boot/traits/ServiceRegistry.h $(AM_CPPFLAGS)
service_registry_SOURCES = $(sources)
service_registry_LDADD = $(libraries)

sys_registry_CPPFLAGS = -include boot/traits/SysRegistry.h $(AM_CPPFLAGS)
sys_registry_SOURCES = $(sources)
sys_registry_LDADD = $(libraries)
