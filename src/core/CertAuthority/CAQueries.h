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


        long getCertCount() const {
            const std::string q = "SELECT COUNT(*) FROM ca_certificate";
            (debug{ } << fmt("CAQuery: {}") << q).log(SOURCE_LOCATION);

            long count = 0;
            db.fetch(q.c_str(), count);
            return count;
        }

        auto getCertificates(const std::string &page, const std::string &item_per_page, const std::string &sort_by, const std::string sort_dir) const {

            std::string q = "SELECT id, common_name, serial, created_at, created_by, valid_after, valid_before, revoked_at FROM ca_certificate ORDER BY " + sort_by + " " + sort_dir;
            if (!page.empty()) {
                const auto p = std::stoul(page);
                const auto i = std::stoul(item_per_page);
                const auto o = p * i; // calculate the offset

                q.append(" LIMIT " + item_per_page + " OFFSET " + std::to_string(o));
            }

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
