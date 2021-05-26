#ifndef _UTILS_DBWRAPPER_H_
#define _UTILS_DBWRAPPER_H_

#include <string>

template<typename DB>
class DbWrapper {

private:

    DB &db;

public:

    DbWrapper(DB &db) : db{ db } {}

    bool dbContentExists(std::string _sSelect, std::string _sFrom, std::string _sWhere, std::string _sWhat)
    {
        std::string sQuery = "SELECT ";
        sQuery += _sSelect + " FROM " + _sFrom + " WHERE " + _sWhere + " = '" + _sWhat + "';";
        //printf("Query: %s\n", sQuery.c_str());
        std::string sResponse;
        db.fetch(sQuery.c_str(), sResponse);
        //printf("sReponse: %s\n", sResponse.c_str());
        return sResponse.size() != 0;
    }

    bool dbContentExists(std::string _sSelect, std::string _sFrom,std::string _sWhere, std::string _sWhat, std::string &_sResponse)
    {
        std::string sQuery = "SELECT ";
        sQuery += _sSelect + " FROM " + _sFrom + " WHERE " + _sWhere + " = '" + _sWhat + "';";
        //printf("Query: %s\n", sQuery.c_str());
        db.fetch(sQuery.c_str(), _sResponse);
        //printf("_sReponse: %s\n", _sResponse.c_str());
        return _sResponse.size() != 0;
    }

    void dbSaveContent(std::string _sTableName, std::string _sColumn, std::string _sValue, bool bMore)
    {
        std::string sQuery = "INSERT INTO ";
        if(!bMore)
            sQuery += _sTableName + " (" + _sColumn + ") " + "VALUES ('" + _sValue + "');";
        else
            sQuery += _sTableName + " (" + _sColumn + ") " + "VALUES (" + _sValue + ");";
        //printf("Query: %s\n", sQuery.c_str());
        db.query(sQuery.c_str());
    }

    void checkAndInsertValue(std::string _sReturnColumn, std::string _sTable, std::string _sCheckColumn, std::string _sCheckValue, std::string &_rRetValue)
    {
        if( !dbContentExists(_sReturnColumn, _sTable, _sCheckColumn, _sCheckValue, _rRetValue))
        {
            dbSaveContent(_sTable, _sCheckColumn, _sCheckValue, false);
            dbContentExists(_sReturnColumn, _sTable, _sCheckColumn, _sCheckValue, _rRetValue);
        }
    }

    void checkAndInsertValues(
        std::string _sReturnColumn,
        std::string _sTable,
        std::string _sCheckColumn,
        std::string _sCheckValue,
        std::string _sInsertColumn,
        std::string _sInsertValue,
        std::string &_rRetValue)
    {
        if( !dbContentExists(_sReturnColumn, _sTable, _sCheckColumn, _sCheckValue, _rRetValue))
        {
            dbSaveContent(_sTable, _sInsertColumn, _sInsertValue, true);
            dbContentExists(_sReturnColumn, _sTable, _sCheckColumn, _sCheckValue, _rRetValue);
        }
    }
};

#endif /* _UTILS_DBWRAPPER_H_ */
