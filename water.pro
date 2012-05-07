QT += core gui opengl

TARGET = water
TEMPLATE = app

CONFIG += debug

QMAKE_CFLAGS += -O3
QMAKE_CFLAGS -= -O2
QMAKE_CXXFLAGS += -O3
QMAKE_CXXFLAGS -= -O2

DEPENDPATH += src
INCLUDEPATH += src

SOURCES += main.cpp \
           mainwindow.cpp \
           glwidget.cpp \
           src/camera.cpp \
           src/waterengine.cpp

HEADERS += mainwindow.h \
           glwidget.h \
           src/camera.h \
           src/vector.h \
           src/waterengine.h
