cmake_minimum_required(VERSION 3.14)

project(dbinit LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql)

add_executable(dbinit
  main.cpp
  dbinit.rc
)

target_link_libraries(dbinit Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Sql dbman)

include(GNUInstallDirs)
install(TARGETS dbinit
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
