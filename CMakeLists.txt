cmake_minimum_required(VERSION 3.13)

project(libicm)

set(CMAKE_CXX_STANDARD 11)

include(GNUInstallDirs)

include_directories(include /opt/local/include/adm/)

set(CMAKE_BUILD_TYPE Debug)

list(APPEND CMAKE_PREFIX_PATH "/opt/local/share/cmake/adm/")

find_package(adm REQUIRED)


add_library(libicm SHARED internal/chunks.cpp internal/icmdocument.cpp internal/InteractiveValueSet.cpp internal/Control.cpp internal/Preset.cpp)

target_link_libraries(libicm PRIVATE adm)


set_target_properties(libicm PROPERTIES
    SOVERSION 1
    PUBLIC_HEADER include/libicm.hpp)

configure_file(libicm.pc.in libicm.pc @ONLY)

target_include_directories(libicm PRIVATE ./include)

install(TARGETS libicm
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_BINARY_DIR}/libicm.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)