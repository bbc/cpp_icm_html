cmake_minimum_required(VERSION 3.13)

project(icm_html_cpp)

set(CMAKE_CXX_STANDARD 11)

include_directories(include /opt/local/include/adm/)

set(CMAKE_BUILD_TYPE Debug)

list(APPEND CMAKE_PREFIX_PATH "/opt/local/share/cmake/adm/")

find_package(adm REQUIRED)




add_executable(icm2html icm2html.cpp internal/chunks.cpp)

target_link_libraries(icm2html PRIVATE adm)