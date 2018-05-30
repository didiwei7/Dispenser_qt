QT       += core
QT       += gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET    = control_lu
TEMPLATE  = lib
DESTDIR   = ../../bin

DEFINES  += CONTROL_LU_LIBRARY
DEFINES  += QT_DEPRECATED_WARNINGS

SOURCES  += \
            control_lu.cpp

HEADERS  += \
            control_lu.h \
            control_lu_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


# DMC
INCLUDEPATH += C:/DMC
win32: LIBS += C:/DMC/LTDMC.lib
