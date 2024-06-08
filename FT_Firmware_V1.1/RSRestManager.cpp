#include "RSRestManager.h"

const std::string RSRESTManager::BASE_URL("https://rshub-be-bffinst-dev.nicewave-a9e9b8ee.westeurope.azurecontainerapps.io/swagger/index.html#/");
const std::string RSRESTManager::DATA_PUSH_ENDPOINT("Instruments/Send");
const std::string RSRESTManager::DEVICE_REGISTER_ENDPOINT("Instruments/Verify");
const std::string RSRESTManager::AUTOSYNC_CHECK_ENDPOINT("Instruments/AutoSyncStatus");

RSRESTManager::RSRESTManager()
{
}

RSRESTManager::~RSRESTManager()
{
}

// Private Methods

// Callback function to write response data
size_t RSRESTManager::writeCallback(void *contents, size_t size, size_t nmemb, std::string *buffer)
{
    size_t totalSize = size * nmemb;
    buffer->append((char*)contents, totalSize);
    return totalSize;
}

// Function to perform an HTTP POST request with JSON body
RSRESTManager::ResponseData RSRESTManager::httpRequest(const std::string& url, const std::string& jsonData, const HttpRequestType reqType)
{
    ResponseData responseData;

    // Initialize libcurl
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = nullptr;
    if (curl)
    {
    std::string requestUrl = url;
        // Set the URL
       // curl_easy_setopt(curl, CURLOPT_URL, requestUrl.c_str());
      //  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

        switch(reqType)
        {
            case GET:
            requestUrl += "?instrumentType=" + jsonData + "&serialNumber=" + jsonData;
                    curl_easy_setopt(curl, CURLOPT_URL, requestUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
                // Fill as per need
            break;
            case PUT:
                    curl_easy_setopt(curl, CURLOPT_URL, requestUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
                // Set the HTTP PUT data
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

                // Set the content type header                
                headers = curl_slist_append(headers, "Content-Type: application/json");
                headers = curl_slist_append(headers, "accept: application/json");
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                break;
            case POST:
                    curl_easy_setopt(curl, CURLOPT_URL, requestUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
                // Set the HTTP POST data
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

                // Set the content type header
                headers = curl_slist_append(headers, "accept: application/json");
                headers = curl_slist_append(headers, "Content-Type: application/json");
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            break;
        }
        // Set the write callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &(responseData.data));

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        // Get the HTTP response code
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &(responseData.httpCode));

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    else
    {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }

    return responseData;
}

// Public Methods

std::string RSRESTManager::registerDevice(const std::string &registrationData)
{
    const std::string requestUrl = BASE_URL + DEVICE_REGISTER_ENDPOINT;
    ResponseData response;
    std::string authToken;
    json jsonResponse;

    response = httpRequest(requestUrl, registrationData, PUT);

    // Check if the request was successful
    if (response.httpCode == 200)
    {
        try
        {
            if(response.data.length() > 0)
            {
                authToken = response.data;
            }
            else
                std::cout << "Invalid tokend received" << std::endl;
        }
        catch(...)
        {
            std::cout << "Exception occurred while parsing registration response" << std::endl;
        }
    }
    else
    {
        std::cerr << "Registration failed with\nHTTP status code: " << response.httpCode << std::endl;
    }

    return authToken;
}

bool RSRESTManager::pushData(const std::string &data)
{
    const std::string requestUrl = BASE_URL + DATA_PUSH_ENDPOINT;
    ResponseData response;

    response = httpRequest(requestUrl, data, POST);

    // Check if the request was successful
    if (response.httpCode == 200)
    {
        return true;
    }
    else
    {
        std::cout << "Failed to push data. HTTP status code: " << response.data << std::endl;
        return false;
    }
}

bool RSRESTManager::Auto_Sync_Check(const std::string &data)
{
    const std::string requestUrl = BASE_URL + AUTOSYNC_CHECK_ENDPOINT;
    ResponseData response;

    response = httpRequest(requestUrl, data, GET);

    // Check if the request was successful
    if (response.httpCode == 200)
    {
    std::cout<<"Server response for Auto Sync"<<response.httpCode<<std::endl;
        return true;
    }
    else
    {
        std::cout << "Failed to get Authentication. HTTP status code: " << response.data << std::endl;
        return false;
    }

}
