cmake_minimum_required(VERSION 3.16)

project(img2txt LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(img2txt
  main.cpp
  cimgtotext.h cimgtotext.cpp
  img2txt.rc
)
target_link_libraries(img2txt Qt${QT_VERSION_MAJOR}::Core uishelter)

include(GNUInstallDirs)
install(TARGETS img2txt
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
