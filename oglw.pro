#-------------------------------------------------
#
# Project created by QtCreator 2015-10-26T23:53:19
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = oglw
TEMPLATE = app


SOURCES += main.cpp\
    oglw.cpp \
    window.cpp \
    glwidget.cpp

HEADERS  += \
    oglw.h \
    window.h \
    glwidget.h

RESOURCES += \
    resources.qrc
