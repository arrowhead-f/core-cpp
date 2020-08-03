#ifndef _DBEXCEPTION_H_
#define _DBEXCEPTION_H_

#include <exception>

namespace db {

    /// Exception thrown by the database.
    class Exception : public std::exception {
        private:
            const int   errc;  ///< The errorcode.
            const char *msg;   ///< The message associated with the error.

            const unsigned i_errno;  ///< The internal error code. Node that negative values in errc implies that this value is set.
            const char    *i_error;  ///< The internal error message.

    public:
        Exception(int code, const char *msg, unsigned int i_errno = 0u, const char *i_error = nullptr)
            : errc{code}, msg{msg}, i_errno{i_errno}, i_error{i_error} {}

        const char *what() const noexcept { return msg; }

        int code() const noexcept { return errc; }

        unsigned internal_errn() const noexcept { return i_errno; }
        const char *internal_error() const noexcept { return i_error; }

    };  // class Exception

}

#endif  // _DBEXCEPTION_H_
