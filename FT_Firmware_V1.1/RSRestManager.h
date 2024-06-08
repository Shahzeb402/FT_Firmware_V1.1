#ifndef __RS_REST_MANAGER__
#define __RS_REST_MANAGER__

#include <iostream>
#include <curl/curl.h>
#include "JsonParser/json.hpp"

using namespace std;
using json = nlohmann::json;

class RSRESTManager
{
    static const std::string BASE_URL;
    static const std::string DATA_PUSH_ENDPOINT;
    static const std::string DEVICE_REGISTER_ENDPOINT;
    static const std::string AUTOSYNC_CHECK_ENDPOINT;

    enum HttpRequestType {
        GET,
        PUT,
        POST
    };

private:
    // Struct to hold HTTP response data
    struct ResponseData
    {
        std::string data;
        long httpCode;
    };

    ResponseData httpRequest(const std::string& url, const std::string& jsonData, const HttpRequestType reqType);
    static size_t writeCallback(void *contents, size_t size, size_t nmemb, std::string *buffer);
public:
    RSRESTManager();
    ~RSRESTManager();

    std::string registerDevice(const std::string &registrationData);
    bool pushData(const std::string &data);
    bool Auto_Sync_Check(const std::string &data);
};

#endif
