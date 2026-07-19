cmake_minimum_required(VERSION 3.20)

project(cpp_inventory_cli VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(inventory_cli
    src/main.cpp
)

target_compile_features(inventory_cli PRIVATE cxx_std_20)

target_compile_options(inventory_cli PRIVATE
    $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
    >
)
