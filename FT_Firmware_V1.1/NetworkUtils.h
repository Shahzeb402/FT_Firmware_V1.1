#ifndef __NETWORK_UTILS__
#define __NETWORK_UTILS__

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>

class NetworkUtils
{
    static const std::string NETWORK_INTERFACE;
private:
    /* data */
public:
    NetworkUtils();
    ~NetworkUtils();

    std::string getMacAddress();
};


#endif