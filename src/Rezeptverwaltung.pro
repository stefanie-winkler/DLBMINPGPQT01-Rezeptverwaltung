QT += core gui widgets

CONFIG += c++17

TARGET = Rezeptverwaltung
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    rezept.cpp \
    rezeptdialog.cpp \
    rezeptmanager.cpp

HEADERS += \
    mainwindow.h \
    rezept.h \
    rezeptdialog.h \
    rezeptmanager.h
