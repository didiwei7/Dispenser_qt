QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG(debug, debug|release): TARGET = QIOd
else:CONFIG(release, debug|release): TARGET = QIO
TEMPLATE  = lib
DESTDIR   = ../../bin

DEFINES  += QIO_LIBRARY


DEFINES  += QT_DEPRECATED_WARNINGS


SOURCES  += \
            qio.cpp

HEADERS  += \
            qio.h \
            qio_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
