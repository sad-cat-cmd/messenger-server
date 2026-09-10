cmake_minimum_required(VERSION 3.14)

project(server_process LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

set(LIBRARY_PATH ${CMAKE_SOURCE_DIR}/../library)
set(LIBRARY_BUILD_PATH ${LIBRARY_PATH}/build)

set(SOURCES
    src/main.cpp
    src/network.cpp
)
set(HEADERS
    include/network.hpp
)

add_executable(server_process
   ${SOURCES} ${HEADERS}
)

target_include_directories(server_process
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${LIBRARY_PATH}/include
)

target_link_libraries(server_process Qt${QT_VERSION_MAJOR}::Core)

include(GNUInstallDirs)
install(TARGETS server_process
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
