QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG(debug, debug|release): TARGET = QMyMsgBoxThreadd
else:CONFIG(release, debug|release): TARGET = QMyMsgBoxThread
TEMPLATE  = lib
DESTDIR   = ../../bin

DEFINES  += QMYMSGBOXTHREAD_LIBRARY
DEFINES  += QT_DEPRECATED_WARNINGS


SOURCES  +=

HEADERS  += \
            qmymsgboxthread.h \
            qmymsgboxthread_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
