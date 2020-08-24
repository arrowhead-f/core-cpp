#ifndef _ARROWHEAD_LOG_H_
#define _ARROWHEAD_LOG_H_

#include <syslog.h>

#define LOG_SEVERITY LOG_WARNING

enum class LogLevel : int {
    LOG_EMERG, LOG_ALERT, LOG_CRIT, LOG_ERR, LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG
};

class NullLog {
    public:
        template<typename T>void print(const T &t) {
            // do nothing
        }
};

class Log {

    private:

        //LOG_EMERG      system is unusable
        //LOG_ALERT      action must be taken immediately
        //LOG_CRIT       critical conditions
        //LOG_ERR        error conditions
        //LOG_WARNING    warning conditions
        //LOG_NOTICE     normal, but significant, condition
        //LOG_INFO       informational message
        //LOG_DEBUG

        int level = LOG_WARNING;

        Log() noexcept {
            openlog("arrowheads", LOG_ODELAY, LOG_USER);
            setlogmask(LOG_SEVERITY);
        }

        Log& setLogLevel(int level) {
            static Log logger;
            logger.level = level;
            return logger;
        }

    public:

        ~Log() {
            closelog();
        }

        Log(const Log&) = delete;
        Log& operator=(const Log&) = delete;

            static Log log { severity };
            static NullLog nlog;

        template<int S>static Log& get() {
            if constexpr (S >= LOG_SEVERITY)
                return log;
            else
                return NullLog{};
        }

        //void syslog(int priority, const char *format, ...);

        template<typename T>void print(const T &t){
            std::ostringstream os;
            os << t;
            syslog(LOG_ERR, os.str().c_str());
        }

        template<>void print<int>(const T &t){
            syslog(LOG_ERR, "%d", t);
        }

        template<>void print<double>(const T &t){
            syslog(LOG_ERR, "%f", t);
        }

};

template<typename T>inline Log& operator<<(Log &l, const T &t) {
    l.__print(t);
    return l;
}

template<typename T>inline NullLog& operator<<(NullLog &l, const T &t) {
    l.__print(t);
    return l;
}

#endif  /* _ARROWHEAD_LOG_H_ */
