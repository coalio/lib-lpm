# The LPM lib includes abstract functions with all of LPM features.
# You can compile this lib independently and use it in your own projects.
include_directories(./lpm ./dependencies)
add_library(lpm-lib
    lpm/packages.cpp
    lpm/pathfinder.cpp
)