#include "DBManager.h"
#include <cstdint>

const std::string DBManager::LTL3500_DATABASE_NAME("ltl3500_database.db");

DBManager::DBManager(RoadSensorModels sensorType)
    : m_dbInstance(nullptr),
      m_sensorType(sensorType)
{
}

DBManager::~DBManager()
{
    if (m_dbInstance != nullptr)
    {
        sqlite3_close(m_dbInstance);
    }
}

int DBManager::callback(void *data, int argc, char **argv, char **azColName)
{
    json *measurements = static_cast<json *>(data);

    json measurement;
    json measurementData; // Nested object for RL.Raw.X, RL.Raw.Y, and RL.Raw.Z

    for (int i = 0; i < argc; ++i)
    {
        std::string columnName(azColName[i]);

        if (columnName == "MeasurementID")
        {
            if (argv[i])
            {
                long long id = std::stoll(argv[i]);
                measurement["measurementId"] = id;
            }
            else
            {
                std::cerr << "MeasurementID is null\n";
            }
        }
        else if (columnName == "SeriesID")
        {
            measurement["seriesId"] = argv[i] ? argv[i] : "";
        }
        else if (columnName == "UserID")
        {
            measurement["userId"] = argv[i] ? argv[i] : "";
        }
        else if (columnName == "Measure_Timestamp")
        {
            measurement["timeStamp"] = argv[i] ? argv[i] : "";
        }
        else if (columnName == "Latitude")
        {
            measurement["latitude"] = argv[i] ? argv[i] : "";
        }
        else if (columnName == "Longitude")
        {
            measurement["longitude"] = argv[i] ? argv[i] : "";
        }
        else if (columnName == "RL.Raw.X")
        {
            measurementData["RL.Raw.X"] = argv[i] ? argv[i] : "";
        }
        else if (columnName == "RL.Raw.Y")
        {
            measurementData["RL.Raw.Y"] = argv[i] ? argv[i] : "";
        }
        else if (columnName == "RL.Raw.Z")
        {
            measurementData["RL.Raw.Z"] = argv[i] ? argv[i] : "";
        }
        else if (columnName == "Tilt")
        {
            measurementData["Tilt"] = argv[i] ? argv[i] : "";
        }
        else if (columnName == "ThirdPartyData")
        {
            measurementData["ThirdPartyData"] = argv[i] ? argv[i] : "";
        }
    }

    // Add the nested measurementData to the measurement object
    measurement["measurementData"] = measurementData;

    // Add the current measurement to the measurements array
    (*measurements).push_back(measurement);

    return 0;
}

// Public Methods

bool DBManager::initDatabaseConnection()
{
    std::string selectedDB;

    switch (m_sensorType)
    {
    case LTL3500:
        selectedDB = LTL3500_DATABASE_NAME;
        break;

    case GRX:

        break;

    default:
        break;
    }

    if (selectedDB.empty())
    {
        std::cout << "Selected Road Sensor has no Database available" << std::endl;
        return false;
    }

    int rc = sqlite3_open(selectedDB.c_str(), &m_dbInstance);
    if (rc)
    {
        std::cout << "Error opening database " << selectedDB << std::endl;
        m_dbInstance = nullptr;
        return false;
    }

    return true;
}

void DBManager::closeDatabaseConnection()
{
    if (m_dbInstance != nullptr)
    {
        sqlite3_close(m_dbInstance);
    }
}


json DBManager::getUnSyncedData(const uint32_t &numOfRows)
{
    json unSynchedData;

    if (m_dbInstance == nullptr)
    {
        return unSynchedData;
    }

    switch (m_sensorType)
    {
    case LTL3500:
        unSynchedData = getUnSyncedDataLtL3500(numOfRows);
        break;
    default:
        break;
    }

    return unSynchedData;
}

json DBManager::getUnSyncedDataLtL3500(const uint32_t &numOfRows)
{
    json unsyncedData; // Overall JSON object
    json measurements; // Array for measurements

    char *errMsg = nullptr;
    std::string query = "SELECT MeasurementID,SeriesID,UserID,Measure_Timestamp,ThirdPartyData,Tilt, `RL.Raw.X`,`RL.Raw.Y`,`RL.Raw.Z`,Latitude,Longitude FROM Measurement as M LEFT JOIN Location as L ON M.MeasurementID = L.ID WHERE M.isSynched = 1 LIMIT " + std::to_string(numOfRows);
    std::cout << "Executing query: " << query << "\n";

    int rc = sqlite3_exec(m_dbInstance, query.c_str(), callback, &measurements, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error executing SQL query: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    else
    {
        std::cout << "Query executed successfully\n";
    }

    // Add serial number and instrument type if available
    unsyncedData["serialNumber"] = DeviceInfo::SN; // Replace with actual serial number
    unsyncedData["instrumentType"] = RoadSensorModels::LTL3500; // Replace with actual instrument type

    // Add measurements array to the main object
    unsyncedData["measurements"] = measurements;


    return unsyncedData;
}

void DBManager::updateIsSynched(const json &unsyncedMeasurementIDs)
{
    for (const auto &measurement : unsyncedMeasurementIDs["ids"])
    {
        // int64_t measurementID = measurement["MeasurementID"];
        int64_t measurementID = measurement.at("MeasurementID").get<int64_t>();
        std::string updateQuery = "UPDATE Measurement SET isSynched = 1 WHERE MeasurementID = " + std::to_string(measurementID) + ";";

        char *errMsg = nullptr;
        int rc = sqlite3_exec(m_dbInstance, updateQuery.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK)
        {
            std::cerr << "Error updating isSynched flag for MeasurementID " << measurementID << ": " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
        else
        {
            std::cout << "MeasurementID " << measurementID << " marked as synched.\n";
        }
    }
}

bool DBManager::UnSync_Data_avl(const std::string& tableName)
{
    char *errMsg = nullptr;
    bool hasData = false;

    std::string query = "SELECT EXISTS (SELECT 1 FROM " + tableName + " WHERE isSynched = 1);";

    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        bool *hasData = static_cast<bool *>(data);
        if (argv[0] != nullptr)
        {
            *hasData = (std::string(argv[0]) == "1");
        }
        else
        {
            *hasData = false;
        }
        return 0;
    };

    int rc = sqlite3_exec(m_dbInstance, query.c_str(), callback, &hasData, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error executing SQL query: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    std::cout << "Has unsynched data in table " << tableName << ": " << std::boolalpha << hasData << "\n";
    return hasData;
}


bool DBManager::columnExists(const std::string& tableName, const std::string& columnName) {
    std::string sql = "PRAGMA table_info(" + tableName + ");";
    sqlite3_stmt* stmt;
    bool exists = false;

    if (sqlite3_prepare_v2(m_dbInstance, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string colName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            if (colName == columnName) {
                exists = true;
                break;
            }
        }
    }
    sqlite3_finalize(stmt);
    return exists;
}

// Function to add a column to the table
void DBManager::addColumn(const std::string& tableName, const std::string& columnName, const std::string& columnType, int defaultValue) {
    std::string sql = "ALTER TABLE " + tableName + " ADD COLUMN " + columnName + " " + columnType + " DEFAULT " + std::to_string(defaultValue) + ";";
    char* errMsg = nullptr;
    if (sqlite3_exec(m_dbInstance, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Error adding column: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Column added successfully." << std::endl;
    }
} 
