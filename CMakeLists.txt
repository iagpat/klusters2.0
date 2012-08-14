project(klusters)
cmake_minimum_required(VERSION 2.8.6)
find_package(Qt3 REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_AUTOMOC TRUE)
add_subdirectory(src)
add_subdirectory(po)
add_subdirectory(doc)

