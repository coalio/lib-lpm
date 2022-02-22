#include <string>
#include <vector>
#include <map>
#include <string_view>
#include "packages.h"
#include "macros.h"
#include "toml11/toml.hpp"

void LPM::PackagesData::load() {
    toml::value data;
    try {
        data = toml::parse(this->path);
    } catch (...) {
        LPM_PRINT_ERROR("Failed to parse: " << this->path);
    }

    this->name = toml::find_or(data, "project", "name", "");
    this->version = toml::find_or(data, "project", "version", "");
    this->description = toml::find_or(data, "project", "description", "");
    this->author = toml::find_or(data, "project", "author", "");
    this->license = toml::find_or(data, "project", "license", "");
    this->homepage = toml::find_or(data, "project", "homepage", "");
    this->repository = toml::find_or(data, "project", "repository", "");
    this->main = toml::find_or(data, "project", "main", "");
    this->lua_version = toml::find_or(data, "project", "lua_version", "");

    if (data.contains("dependencies")) {
        this->dependencies = toml::find<
            std::map<std::string, std::string>
        >(data, "dependencies");
    }

    LPM_PRINT("Loaded data for project: " << this->name);

    // print all of the parsed data
    LPM_PRINT("this->name : " << this->name);
    LPM_PRINT("this->version : " << this->version);
    LPM_PRINT("this->description : " << this->description);
    LPM_PRINT("this->author : " << this->author);
    LPM_PRINT("this->license : " << this->license);
    LPM_PRINT("this->homepage : " << this->homepage);
    LPM_PRINT("this->repository : " << this->repository);
    LPM_PRINT("this->main : " << this->main);
    LPM_PRINT("this->lua_version : " << this->lua_version);
    LPM_PRINT("this->dependencies.size() : " << this->dependencies.size());
    for (auto& dependency : this->dependencies) {
        LPM_PRINT("this->dependencies : " << dependency.first << " " << dependency.second);
    }
}