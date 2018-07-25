QT       += core
QT       += gui
QT       += sql
QT       += xml
QT       += network
QT       += serialport
QT       += multimedia
QT       += charts
QT       += concurrent
QT       += winextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET    = ui
TEMPLATE  = app
DESTDIR   = ../bin

CONFIG   += c++11

VERSION   = 1.0.0.0
QMAKE_TARGET_PRODUCT = Dispenser
QMAKE_TARGET_COMPANY = didiwei.com
QMAKE_TARGET_DESCRIPTION = Dispenser
QMAKE_TARGET_COPYRIGHT   = Didiwei

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
            main.cpp                        \
            mainwindow/mainwindow.cpp       \
            mainwindow/mainwidget.cpp       \
            communication/communication.cpp \
            adt/adtcontrol.cpp              \
            io/io.cpp                       \
            motor/motor.cpp                 \
            motor/point/pointdebug.cpp      \
            vision/vision.cpp               \
            photo/photo.cpp                 \
            operation/operation.cpp         \
            workflow/workflow.cpp

HEADERS += \
            mainwindow/mainwindow.h         \
            mainwindow/mainwidget.h         \
            communication/communication.h   \
            adt/adtcontrol.h                \
            io/io.h                         \
            motor/motor.h                   \
            motor/point/pointdebug.h        \
            vision/vision.h                 \
            photo/photo.h                   \
            operation/operation.h           \
            workflow/workflow.h

RC_FILE  += myico.rc

# SDK
win32:CONFIG(debug, debug|release):{
    LIBS += ../bin/QFlexd.lib
    LIBS += ../bin/QMyPlugind.lib
}else:win32:CONFIG(release, debug|release):{
    LIBS += ../bin/QFlex.lib
    LIBS += ../bin/QMyPlugin.lib
}

# OpenCV
INCLUDEPATH += C:/opencv/opencv300/include
INCLUDEPATH += C:/opencv/opencv300/include/opencv/
INCLUDEPATH += C:/opencv/opencv300/include/opencv2/
win32:CONFIG(debug, debug|release): LIBS += C:/opencv/opencv300/x86/vc12/lib/opencv_world300d.lib
else:win32:CONFIG(release, debug|release): LIBS += C:/opencv/opencv300/x86/vc12/lib/opencv_world300.lib

# Eigen
INCLUDEPATH += C:/myPlugin/Eigen3

# ADT
INCLUDEPATH += C:/Motion/ADT
win32: LIBS += C:/Motion/ADT/adt8949.lib

