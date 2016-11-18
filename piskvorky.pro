# -------------------------------------------------
# Project created by QtCreator 2011-04-14T19:34:16
# -------------------------------------------------
QT += core \
    gui \
    network
TARGET = piskvorky
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    item.cpp \
    board.cpp \
    server.cpp \
	client.cpp
HEADERS += mainwindow.h \
    item.h \
    board.h \
    server.h \
	client.h
FORMS += mainwindow.ui
RESOURCES += resource.qrc
