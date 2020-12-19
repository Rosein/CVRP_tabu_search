cmake_minimum_required(VERSION 3.14)
project(VRP_tabu_search)

add_executable(VRP projekt_2.cpp)

include(CPM.cmake)

CPMAddPackage(
    NAME matplotplusplus
    GITHUB_REPOSITORY alandefreitas/matplotplusplus
    GIT_TAG origin/master # or whatever tag you want
)

target_link_libraries(VRP PUBLIC matplot)
