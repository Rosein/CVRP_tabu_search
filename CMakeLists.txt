cmake_minimum_required(VERSION 3.14)
project(CVRP_tabu_search)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(CVRP cvrp_tabu_search.cpp)

include(cmake/CPM.cmake)

CPMAddPackage(
    NAME matplotplusplus
    GITHUB_REPOSITORY alandefreitas/matplotplusplus
    GIT_TAG origin/master # or whatever tag you want
)

target_link_libraries(CVRP PUBLIC matplot)
