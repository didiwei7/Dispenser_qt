QT       += core gui
QT       += sql
QT       += xml
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG(debug, debug|release): TARGET = QNavigationBard
else:CONFIG(release, debug|release): TARGET = QNavigationBar
TEMPLATE  = lib
DESTDIR   = ../../bin

DEFINES  += QNAVIGATIONBAR_LIBRARY
DEFINES  += QT_DEPRECATED_WARNINGS

SOURCES  += \
            qnavigationbar.cpp

HEADERS  += \
            qnavigationbar.h \
            qnavigationbar_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
