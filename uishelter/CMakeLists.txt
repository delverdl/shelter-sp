cmake_minimum_required(VERSION 3.16)

project(uishelter VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

add_library(uishelter SHARED
  uishelter_global.h
  uishelter.cpp
  uishelter.h
  cwgtuserlist.h
  cwgtuserlist.cpp
  cwgtuserlist.ui
  cnewuser.h cnewuser.cpp cnewuser.ui
  cusercheck.h cusercheck.cpp cusercheck.ui
)

target_link_libraries(uishelter PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
   Qt${QT_VERSION_MAJOR}::Sql dbman)

target_compile_definitions(uishelter PRIVATE UISHELTER_LIBRARY)

target_include_directories(uishelter INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

include(GNUInstallDirs)
install(TARGETS uishelter
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
