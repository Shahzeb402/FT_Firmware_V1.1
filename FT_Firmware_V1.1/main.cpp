#include <stdio.h>
#include "JsonParser/json.hpp"
#include "DBManager.h"
#include "TokenManager.h"
#include "RSRestManager.h"

using namespace std;
using json = nlohmann::json;

int main()
{
    std::cout << "..Welcome to Force Technology.." << endl;
    const RoadSensorModels sensorModel = LTL3500;
    json autoData;
    autoData["instrumentType"] = sensorModel;
    autoData["serialNumber"] = DeviceInfo::SN;
    json pushdata;
    std::string tableName = "Measurement";
    std::string columnName = "isSynched";
    std::string columnType = "INTEGER";
    int defaultValue = 0;
    TokenManager tokenMgr;
    RSRESTManager restApiMgr;
    DBManager databaseConnection(sensorModel);
    databaseConnection.initDatabaseConnection();
    bool authTokenExists = tokenMgr.authTokenExist();
    if (!authTokenExists)
    {
        tokenMgr.DeviceRegistration();
    } else {
    std::cout<<"Device is already registered"<<std::endl;
    }
    const std::string runtimeToken = tokenMgr.generateRuntimeToken();
    if (runtimeToken.empty())
    {
        cout << "Could not generate runtime token" << endl;
        return 1;
    } else {
    std::cout<<"Run Time generated token is : "<< runtimeToken <<std::endl;
    }
    std::cout<<"Auto Sync Data :"<<autoData.dump()<<std::endl;
    if(!restApiMgr.Auto_Sync_Check(autoData.dump())){
    cout<<"could not get Authentication from server. Exiting..."<<endl;
    }else{
    cout <<"You are good to go brother"<<std::endl;
    }
    if (!databaseConnection.columnExists(tableName, columnName))
    {
        databaseConnection.addColumn(tableName, columnName, columnType, defaultValue);
    } else {
    std::cout<<"Column Already Exist in Measurement"<<std::endl;
    }
    if (databaseConnection.UnSync_Data_avl("Measurement"))
    {
        pushdata = databaseConnection.getUnSyncedDataLtL3500(1);
        // pushdata = databaseConnection.getUnSyncedData(1);
        if (pushdata.empty())
        {
            cout << "No Data returned from DB.Exiting...." << endl;
            return 1;
        }
        else
        {
        pushdata["runtimeToken"] = runtimeToken;
            cout << "Final JSON Data get Successfully......" << endl
                 << pushdata.dump() << endl;
        }
    }
    else
    {
        cout << "Currently No New data Available......" << endl;
    }
    if(!restApiMgr.pushData(pushdata.dump())){
    cout<<"could not push data to server. Exiting..."<<endl;
    }

    // databaseConnection.updateIsSynched(pushdata);

    databaseConnection.closeDatabaseConnection();

    return 0;
}
