#include "errors.h"
#include "macros.h"

void LPM::Errors::ErrorList::add(const std::string& stage, const std::string& error) {
    errors[stage].emplace_back(error);
}

void LPM::Errors::ErrorList::print() {
    for (auto& error : errors) {
        LPM_PRINT_ERROR("during " << error.first << ":");
        for (auto& message : error.second) {
            LPM_PRINT_ERROR("\t" << message);
        }
    }
}