#ifndef _LOG_H_
#define _LOG_H_

#include <syslog.h>

#define LOG_SEVERITY LOG_WARNING

class NullLog {
    public:
        template<typename T>void print(const T &t) {
        }
};

class Log {

    private:

        static constexpr int severity = LOG_SEVERITY;

        //LOG_EMERG      system is unusable
        //LOG_ALERT      action must be taken immediately
        //LOG_CRIT       critical conditions
        //LOG_ERR        error conditions
        //LOG_WARNING    warning conditions
        //LOG_NOTICE     normal, but significant, condition
        //LOG_INFO       informational message
        //LOG_DEBUG

        Log() noexcept {
            openlog("arrowheads", LOG_ODELAY, LOG_USER);
            setlogmask(severity);
        }

    public:

        ~Log() {
            closelog();
        }

        Log(const Log&) = delete;
        Log& operator=(const Log&) = delete;

        static Log& get(int severity) {
            static Log log { LOG_WARNING };

            if constexpr (severity > LOG_SEVERITY)
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

template<typename T>Log& operator<<(Log &l, const T &t) {
    l.__print(t);
    return l;
}

#endif  /* _LOG_H_ */