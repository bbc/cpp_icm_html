#Start
cmake_minimum_required(VERSION 3.10)
project(icm_proj)
set(CMAKE_BUILD_TYPE Debug)

#Include ADM
include_directories(include /opt/local/include/adm/)
list(APPEND CMAKE_PREFIX_PATH "/opt/local/share/cmake/adm/")
find_package(adm REQUIRED)

#Build library
add_library(icm SHARED internal/chunks.cpp internal/icmdocument.cpp internal/InteractiveValueSet.cpp internal/Control.cpp internal/Preset.cpp)

#Link against libadm
target_link_libraries(icm PRIVATE adm)

#Install library
install(TARGETS icm DESTINATION lib)
install(FILES include/libicm.hpp include/Preset.hpp include/chunks.hpp include/Control.hpp include/InteractiveValueSet.hpp include/common.hpp include/icmdocument.hpp DESTINATION include/icm)



