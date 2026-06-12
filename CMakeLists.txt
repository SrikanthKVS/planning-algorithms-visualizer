cmake_minimum_required(VERSION 3.20)
project(planning_algs VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Dependencies ────────────────────────────────────────────────
find_package(Threads REQUIRED)

# Crow needs Asio — tell it to use standalone (not Boost)
add_definitions(-DCROW_USE_STANDALONE_ASIO)
add_definitions(-DASIO_STANDALONE)

# ── Server binary ────────────────────────────────────────────────
add_executable(planning_algs_server
        server/main.cpp
        include/planners/grid_types.h
        src/planners/bfs.cpp
        include/planners/bfs.h
        src/planners/dfs.cpp
        include/planners/dfs.h
)

target_include_directories(planning_algs_server PRIVATE
    third_party/crow    # crow_all.h lives here
    third_party         # future: nanoflann, etc.
    include
    /usr/include
)

target_link_libraries(planning_algs_server PRIVATE Threads::Threads)

# Warnings
target_compile_options(planning_algs_server PRIVATE -Wall -Wextra)

message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
