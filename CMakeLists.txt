cmake_minimum_required(VERSION 3.16)
project(RezeptVerwaltung LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(RezeptVerwaltung
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/rezept.cpp
    src/rezept.h
    src/rezeptmanager.cpp
    src/rezeptmanager.h
    src/rezeptdialog.cpp
    src/rezeptdialog.h
)

target_link_libraries(RezeptVerwaltung PRIVATE Qt6::Widgets)
