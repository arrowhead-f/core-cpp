#ifndef _CORE_CA_MGMT_H_
#define _CORE_CA_MGMT_H_


#include "http/crate/Request.h"
#include "http/crate/Response.h"


template<typename DB>
class Mgmt {

    private:

        static constexpr std::size_t preflen = 5;  ///< The length of the path prefix, i.e., the length of "/mgmt"

        DB &db;

    public:

        Mgmt(DB &db) : db{ db } {}

        Response handleGET(Request &&req) {
            return Response::from_stock(http::status_code::NotFound);
        }

        Response handlePOST(Request &&req) {
            return Response::from_stock(http::status_code::NotFound);
        }

        Response handlePATCH(Request &&req) {
            return Response::from_stock(http::status_code::NotFound);
        }

        Response handlePUT(Request &&req) {
            return Response::from_stock(http::status_code::NotFound);
        }

        Response handleDELETE(Request &&req) {
            return Response::from_stock(http::status_code::NotFound);
        }

};

#endif  /* _CORE_CA_MGMT_H_ */
