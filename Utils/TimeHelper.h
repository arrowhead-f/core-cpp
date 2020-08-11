#ifndef CORE_CPP_TIMEHELPER_H
#define CORE_CPP_TIMEHELPER_H

///STD libs
#include <ctime>

///Class made to ease time arithmetics
class TimeHelper {
public:

    ///Calculates time offset in minutes from now
    /*!
     *
     * @param offsetMinutes
     * @return numericDate
     */
    long offsetFromNow(float offsetMinutes)
    {
        time_t numericDate = time(0);
        float secondsOffset = offsetMinutes *60;
        numericDate -= (long)secondsOffset;
        return numericDate;
    }
};


#endif //CORE_CPP_TIMEHELPER_H
