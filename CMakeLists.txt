cmake_minimum_required(VERSION 3.14)

project(server_process LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(server_process
   src/main.cpp
)

set(SOURCES
    src/main.cpp
)

set(HEADERS
    #
)

target_link_libraries(server_process Qt${QT_VERSION_MAJOR}::Core)

include(GNUInstallDirs)
install(TARGETS server_process
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
