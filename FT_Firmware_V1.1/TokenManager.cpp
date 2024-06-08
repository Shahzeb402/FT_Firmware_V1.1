#include "TokenManager.h"
#include "NetworkUtils.h"
#include "RSRestManager.h"

const std::string TokenManager::TOKEN_DIR_PATH = "/home/RSSync1";
const std::string TokenManager::TOKEN_FILE = "RSSyncToken";

TokenManager::TokenManager()
{
}

TokenManager::~TokenManager()
{
}

// Private Methods

std::string TokenManager::sha256(const std::string &input)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.length());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}

// Public Methods

bool TokenManager::authTokenExist()
{
    if (!FSUtils::directoryExists(TOKEN_DIR_PATH))
    {
        if (!FSUtils::createDirectory(TOKEN_DIR_PATH))
        {
            return false;
        }
    }

    const std::string tokenFullFileName = FSUtils::joinPath(TOKEN_DIR_PATH, TOKEN_FILE);

    if (!tokenFullFileName.empty())
        return FSUtils::fileExists(tokenFullFileName);
    else
        return false;
}

std::string TokenManager::generateRuntimeToken()
{
    const std::string tokenFullFileName = FSUtils::joinPath(TOKEN_DIR_PATH, TOKEN_FILE);

    if (tokenFullFileName.empty())
        return std::string();

    if (!FSUtils::fileExists(tokenFullFileName))
        return std::string();

    const std::string authToken = FSUtils::readFile(tokenFullFileName);

    if (authToken.empty())
    {
        std::cout << "Could not read auth token for runtime token generation." << std::endl;
        return std::string();
    }

    const std::string deviceSN = std::to_string(DeviceInfo::SN);

    const std::string runtimeToken = sha256(deviceSN + authToken);

    return runtimeToken;
}

int TokenManager::DeviceRegistration()
{
    NetworkUtils networkManager;
    RSRESTManager restApiMgr;
    TokenManager tokenMgr;
    json registrationData;
    string macAddress = networkManager.getMacAddress();
    //const RoadSensorModels sensorModel = LTL3500;
    if (macAddress.empty())
    {
        cout << "No network address detected on this device. Exiting..." << endl;
        return 1;
    }
    registrationData["serialNumber"] = DeviceInfo::SN;
    registrationData["macAddress"] = macAddress;
    registrationData["instrumentType"] = RoadSensorModels::LTL3500;
    cout << "Registration request data: " << registrationData.dump() << endl;
    const std::string newToken = restApiMgr.registerDevice(registrationData.dump());
    if (newToken.empty())
    {
        cout << "Could not get new authentication token. Exiting..." << endl;
        return 1;
    }

    if (!tokenMgr.saveToken(newToken))
    {
        cout << "Could not save new authentication token. Exiting..." << endl;
        return 1;
    }

    cout << "Registration successful" << endl;
}

bool TokenManager::saveToken(const std::string &token)
{
    const std::string tokenFullFileName = FSUtils::joinPath(TOKEN_DIR_PATH, TOKEN_FILE);

    if (tokenFullFileName.empty())
        return false;

    return FSUtils::writeFile(tokenFullFileName, token);
}
