#ifndef __DB_MANAGEMENT__
#define __DB_MANAGEMENT__

#include <iostream>
#include "sqlite3/sqlite3.h"
#include "Common.h"
#include "JsonParser/json.hpp"

using json = nlohmann::json;

class DBManager
{
    // Typedefs
    typedef sqlite3 *DBInstance;

    // Constants
    static const std::string LTL3500_DATABASE_NAME;
    static const std::string GRX_DATABASE_NAME;

private:
    DBInstance m_dbInstance;
    RoadSensorModels m_sensorType;
    struct CallbackData
    {
        json &unSyncedData;
        json &measurementData;
    };

public:
    DBManager(RoadSensorModels sensorType);
    ~DBManager();


    std::vector<std::string> tables;
    bool initDatabaseConnection();
    void closeDatabaseConnection();
    static int callback(void *data, int argc, char **argv, char **azColName);
    json getUnSyncedData(const uint32_t &numOfRows);
    // json getUnSyncedDataLtL3500(const uint32_t &numOfRows);
    json getUnSyncedDataLtL3500(const uint32_t &numOfRows);
    void updateIsSynched(const json &unsyncedMeasurementIDs);
    bool UnSync_Data_avl(const std::string& tableName);
    bool columnExists(const std::string& tableName, const std::string& columnName);
    void addColumn(const std::string& tableName, const std::string& columnName, const std::string& columnType, int defaultValue);

};

#endif