#ifndef _TEST_JSONCOMP_H_
#define _TEST_JSONCOMP_H_


#include <string>


#include "json11/json11.hpp"


/// CChecks whether the json object's string represenatation is valid.
/// \param str    the json object's string represenatation
/// \return       true, if the string represenation can be parsed as a valid json object
inline bool JsonParse(const std::string &str) {

    using namespace json11;

    std::string err;

    Json::parse(str, err);
    if(!err.empty())
        return false;

    return true;
}


/// Compares two json object strored in string.
/// \param lhs    the first json object's string represenatation
/// \param rhs    the second json object's string representation
/// \return       true, if the two json objects are the same
inline int JsonCompare(const std::string &lhs, const std::string &rhs) {

    using namespace json11;

    std::string err;

    const auto jlhs = Json::parse(lhs, err);
    if(!err.empty())
        return false;

    err.clear();

    const auto jrhs = Json::parse(rhs, err);
    if(!err.empty())
        return false;

    return jlhs == jrhs;
}

#endif  /* _TEST_JSONCOMP_H_ */
