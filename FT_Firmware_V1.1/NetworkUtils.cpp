#include "NetworkUtils.h"

const std::string NetworkUtils::NETWORK_INTERFACE("enp0s3");

NetworkUtils::NetworkUtils()
{
}

NetworkUtils::~NetworkUtils()
{
}

// Public Methods

std::string NetworkUtils::getMacAddress()
{
    std::ifstream file("/sys/class/net/" + NETWORK_INTERFACE + "/address");

    if (!file) {
        std::cerr << "Failed to open file." << std::endl;
        return std::string();
    }

    std::string mac;
    file >> mac;
    return mac;
}