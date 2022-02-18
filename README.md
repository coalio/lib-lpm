# lpm-lib

This library contains all the functionality of the Lua Package Manager.

## Integration

To integrate the library into your project, you need to follow these steps:

-   Clone the repository:
    ```bash
    git clone https://github.com/coalio/lib-lpm lib-lpm
    ```
-   Add the library to your project using CMake:

    ```cmake
    add_subdirectory(lib-lpm/)
    target_link_libraries(your_target lib-lpm)
    ```

    Optionally, you can also compile the library separately:

    ```bash
    cmake .
    make
    ```

    And then link this library when you compile your project:

    ```bash
    g++ -std=c++11 -Ilib-lpm/include -Llib-lpm/lib -llpm-lib your_program.cpp -llpm-lib
    ```
