QT       += core gui
QT       += sql
QT       += xml
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG(debug, debug|release): TARGET = QMySliderd
else:CONFIG(release, debug|release): TARGET = QMySlider
TEMPLATE  = lib
DESTDIR   = ../../bin


DEFINES  += QMYSLIDER_LIBRARY
DEFINES  += QT_DEPRECATED_WARNINGS

SOURCES  += \
            qmyslider.cpp

HEADERS  += \
            qmyslider.h \
            qmyslider_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    myslider.qrc


