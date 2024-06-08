#ifndef __TOKEN_MANAGEMENT__
#define __TOKEN_MANAGEMENT__

#include <iostream>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include "FSUtils.h"
#include "Common.h"

class TokenManager
{
private:
    std::string sha256(const std::string& input);
    
public:

    static const std::string TOKEN_DIR_PATH;
    static const std::string TOKEN_FILE;

    TokenManager();
    ~TokenManager();

    bool authTokenExist();
    bool saveToken(const std::string &token);
    std::string generateRuntimeToken();
    int DeviceRegistration();
};

#endif