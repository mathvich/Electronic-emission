#-------------------------------------------------
#
# Project created by QtCreator 2017-06-10T16:44:59
#
#-------------------------------------------------

QT       += core gui opengl
LIBS += -LC:\Qt\Qt5.5.1\5.5\mingw492_32\lib\libQt5OpenGL.a -lopengl32
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = modeling
TEMPLATE = app


SOURCES += main.cpp\
        window.cpp \
    glwidget.cpp \
    point.cpp \
    qcustomplot.cpp \
    plotwidget.cpp \
    particle.cpp \
    world.cpp \
    processingthread.cpp \
    rainbow.cpp

HEADERS  += window.h \
    glwidget.h \
    point.h \
    qcustomplot.h \
    plotwidget.h \
    particle.h \
    world.h \
    processingthread.h \
    rainbow.h
