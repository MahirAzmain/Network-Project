#include "RPLCommon.h"

IPv6Address IPv6Address::fromString(const std::string& str) {
    IPv6Address addr;
    std::stringstream ss(str);
    std::string token;
    int idx = 0;

    while (std::getline(ss, token, ':') && idx < 16) {
        if (token.length() > 0) {
            unsigned int val = std::stoi(token, nullptr, 16);
            addr.bytes[idx] = (val >> 8) & 0xFF;
            addr.bytes[idx + 1] = val & 0xFF;
            idx += 2;
        }
    }

    return addr;
}
