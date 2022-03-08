#pragma once
#include <map>
#include <string>
#include <vector>

namespace LPM::Errors {
    class ErrorList {
    public:
        std::map<std::string, std::vector<std::string>> errors;

        void add(const std::string& stage, const std::string& error);
        void print();
    };
}