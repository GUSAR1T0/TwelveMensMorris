#-------------------------------------------------
#
# Project created by QtCreator 2015-11-19T22:44:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TwelveMensMorris
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    newgame.cpp \
    exitgame.cpp \
    algorithm.cpp \
    gameover.cpp \
    settings.cpp \
    rules.cpp \
    goto_mainpage.cpp

HEADERS  += mainwindow.h \
    newgame.h \
    exitgame.h \
    algorithm.h \
    gameover.h \
    settings.h \
    rules.h \
    goto_mainpage.h

FORMS    += mainwindow.ui \
    newgame.ui \
    exitgame.ui \
    gameover.ui \
    settings.ui \
    rules.ui \
    goto_mainpage.ui

RESOURCES += img.qrc \
    htm.qrc

RC_FILE = icon.rc
