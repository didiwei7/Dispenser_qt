QT       += core
QT       += gui
QT       += sql
QT       += xml
QT       += network
QT       += serialport
QT       += multimedia
QT       += charts
QT       += concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG(debug, debug|release): TARGET = QMyPlugind
else:CONFIG(release, debug|release): TARGET = QMyPlugin
DESTDIR   = ../../bin
TEMPLATE  = lib

DEFINES  += QMYPLUGIN_LIBRARY
DEFINES  += QT_DEPRECATED_WARNINGS

SOURCES  += qio/qio.cpp \
            qmyslider/qmyslider.cpp \
            qnavigationbar/qnavigationbar.cpp \
            qthermometer/qthermometer.cpp \
            qcommunication/qcommunication.cpp \
            qflowlayout/qflowlayout.cpp


HEADERS  += qmyplugin_global.h   \
            qmyplugin.h          \
            qio/qio.h \
            qmyslider/qmyslider.h \
            qnavigationbar/qnavigationbar.h \
            qthermometer/qthermometer.h \
            qcommunication/qcommunication.h \
            qflowlayout/qflowlayout.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    res.qrc
