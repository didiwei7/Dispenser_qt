QT       += core
QT       += gui
QT       += testlib
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET    = T01_LEADSHINE
TEMPLATE  = lib
DESTDIR   = ../../bin

DEFINES  += T01_LEADSHINE_LIBRARY

DEFINES  += QT_DEPRECATED_WARNINGS

SOURCES  += \
    basiio.cpp \
    cmotor.cpp

HEADERS  += \
    baseio.h \
    cmotor.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

# DMC
INCLUDEPATH += C:/DMC
win32: LIBS += C:/DMC/LTDMC.lib
win32: LIBS += C:/DMC/IOC0640.lib
