#ifndef _CORE_CAQUERIES_H_
#define _CORE_CAQUERIES_H_


#include "utils/logger.h"


template<typename DB>class CAQueries {

    private:

        DB &db;

    public:

        CAQueries(DB &db) : db{ db } {}

        CAQueries(const CAQueries&) = delete;
        CAQueries(CAQueries&&) = delete;
        CAQueries& operator=(const CAQueries&) = delete;
        CAQueries& operator=(CAQueries&&) = delete;



        auto getTrustedKey(const std::string &hash) const {
            const std::string q = "SELECT id, created_at, description FROM ca_trusted_key WHERE hash = '" + hash + "'";

            (debug{ } << fmt("CAQuery: {}") << q).log(SOURCE_LOCATION);

            return db.fetch(q.c_str());
        }

        auto getCertificate(const std::string &hash) const {
            const std::string q = "SELECT id, created_at, description FROM ca_trusted_key WHERE hash = '" + hash + "'";

            (debug{ } << fmt("CAQuery: {}") << q).log(SOURCE_LOCATION);

            return db.fetch(q.c_str());
        }


};

template<typename DB>
CAQueries(DB&) -> CAQueries<DB>;

template<typename DB>auto CAQuery(DB &db) {
    return CAQueries{ db };
}

#endif /* _CORE_CAQUERIES_H_ */
