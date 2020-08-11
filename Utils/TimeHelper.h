#ifndef CORE_CPP_TIMEHELPER_H
#define CORE_CPP_TIMEHELPER_H

///STD libs
#include <ctime>

///Class made to ease time arithmetics
class TimeHelper {
public:

   ///Calculates offset in the past
   /*!
    *
    * @param offsetMinutes
    */
    long offsetInThePast(float offsetMinutes)
    {
        return offsetFromNow(-offsetMinutes);
    }

    ///Calculates offset in the future
    /*!
     *
     * @param offsetMinutes
     */
    long offsetInTheFuture(float offsetMinutes)
    {
        return offsetFromNow(offsetMinutes);
    }

    ///Calculates offset from now
    /*!
     *
     * @param offsetMinutes
     */
    long offsetFromNow(float offsetMinutes)
    {
        time_t numericDate = time(0);
        float secondsOffset = offsetMinutes *60;
        numericDate += (long)secondsOffset;
        return numericDate;
    }
};


#endif //CORE_CPP_TIMEHELPER_H
