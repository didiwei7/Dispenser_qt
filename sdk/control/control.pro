QT       += core
QT       += gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET    = control
TEMPLATE  = lib
DESTDIR   = ../../bin

DEFINES += CONTROL_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        control.cpp \
    axis.cpp \
    motor.cpp

HEADERS += \
        control.h \
        control_global.h \ 
        axis.h \
    motor.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

# DMC
INCLUDEPATH += C:/DMC
win32: LIBS += C:/DMC/LTDMC.lib
win32: LIBS += C:/DMC/IOC0640.lib
