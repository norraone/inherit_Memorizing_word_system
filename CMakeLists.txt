cmake_minimum_required(VERSION 3.29)
project(Demo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "D:/Qt/Qt5.14.2/5.14.2/mingw73_64")

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(Demo main.cpp
        mainwindow.cpp
        mainwindow.h
)
target_link_libraries(Demo
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)
