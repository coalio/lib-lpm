# The LPM lib includes abstract functions with all of LPM features.
# You can compile this lib independently and use it in your own projects.
include_directories(./lpm ./dependencies)
add_library(lpm-lib
    lpm/dependencies.cpp
    lpm/manifests.cpp
    lpm/errors.cpp
    lpm/pathfinder.cpp
    lpm/requests.cpp
    lpm/utils.cpp
    lpm/env.cpp
)