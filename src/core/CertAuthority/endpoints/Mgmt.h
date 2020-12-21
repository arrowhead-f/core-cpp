#ifndef _ARROWHEAD_CERTIFICATEAUTHORITY_MGMT_H_
#define _ARROWHEAD_CERTIFICATEAUTHORITY_MGMT_H_

template<typename DB>
class Mgmt {

    private:

        DB &db;

    public:

        Mgmt() : db{ db } {}

        int dispatch() {


        }
};

#endif  /* _ARROWHEAD_CERTIFICATEAUTHORITY_MGMT_H_ */
