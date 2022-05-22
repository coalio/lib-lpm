#pragma once
#include <openssl/sha.h>
#include <gmpxx.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

namespace LPM::Hashing {
    std::string djb2(const::std::string& str);

    std::string sha256(const::std::string& str);
}