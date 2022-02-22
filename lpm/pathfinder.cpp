#include "pathfinder.h"

std::string LPM::PathFinder::locate_config() {
    // TODO: Implement this function
    // Should search for the config file in the following order:
    // 1. Get env LPM_DIR and then $LPM_DIR/config.toml
    // 2. Check $HOME/.lpm/config.toml
    // 3. Check /home/$USER/.lpm/config.toml
    // 4. Check ./config.toml. If its the last one, warn the user that it's a bad idea
    //    to place the config file in a non-standard location.

    return "";
}