QT       += core gui
QT       += network
QT       += serialport
QT       += concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG(debug, debug|release): TARGET = QCommunicationd
else:CONFIG(release, debug|release): TARGET = QCommunication
TEMPLATE  = lib
DESTDIR   = ../../bin

DEFINES  += QCOMMUNICATION_LIBRARY
DEFINES  += QT_DEPRECATED_WARNINGS

SOURCES  += \
            qcommunication.cpp

HEADERS  += \
            qcommunication.h \
            qcommunication_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
