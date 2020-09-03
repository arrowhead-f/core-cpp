#ifndef _ARROWHEAD_LOG_H_
#define _ARROWHEAD_LOG_H_

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#ifdef __func__
  #define SOURCE __FILE__, __LINE__, __func__
#else
  #define SOURCE __FILE__, __LINE__
#endif

#ifdef USE_SYSLOG
  #include "logger/syslog_impl.h"
#endif

#ifdef USE_DEVLOG
  #include "logger/syslog_impl.h"
#endif

#endif  /* _ARROWHEAD_LOG_H_ */
