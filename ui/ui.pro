QT       += core gui
QT       += sql
QT       += xml
QT       += network
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
    LIBS += ../bin/QNavigationBard.lib
    LIBS += ../bin/QMySliderd.lib
    LIBS += ../bin/QCommunicationd.lib
    LIBS += ../bin/QIOd.lib
}else:win32:CONFIG(release, debug|release):{
    LIBS += ../bin/QFlex.lib
    LIBS += ../bin/QNavigationBar.lib
    LIBS += ../bin/QMySlider.lib
    LIBS += ../bin/QCommunicationd.lib
    LIBS += ../bin/QIO.lib
}

# QFlex
#INCLUDEPATH += C:/Qt/QtMyCtrls/include
#win32:CONFIG(debug, debug|release): LIBS += C:/Qt/QtMyCtrls/lib/QFlexd.lib
#else:win32:CONFIG(release, debug|release): LIBS += C:/Qt/QtMyCtrls/lib/QFlex.lib

# QNavigationBar
#INCLUDEPATH += C:/Qt/QtMyCtrls/include/QNavigationBar
#win32:CONFIG(debug, debug|release): LIBS += C:/Qt/QtMyCtrls/lib/QNavigationBard.lib
#else:win32:CONFIG(release, debug|release): LIBS += C:/Qt/QtMyCtrls/lib/QNavigationBar.lib

# QMySlider
#INCLUDEPATH += C:/Qt/QtMyCtrls/include/QMySlider
#win32:CONFIG(debug, debug|release): LIBS += C:/Qt/QtMyCtrls/lib/QMySliderd.lib
#else:win32:CONFIG(release, debug|release): LIBS += C:/Qt/QtMyCtrls/lib/QMySlider.lib

# OpenCV
INCLUDEPATH += C:/opencv300/opencv/build/include
INCLUDEPATH += C:/opencv300/opencv/build/include/opencv/
INCLUDEPATH += C:/opencv300/opencv/build/include/opencv2/
win32:CONFIG(debug, debug|release): LIBS += C:/opencv300/opencv/build/x86/vc12/lib/opencv_world300d.lib
else:win32:CONFIG(release, debug|release): LIBS += C:/opencv300/opencv/build/x86/vc12/lib/opencv_world300.lib

# Eigen
INCLUDEPATH += C:/eigen3


# DMC
# INCLUDEPATH += C:/DMC
# win32: LIBS += C:/DMC/LTDMC.lib
# win32: LIBS += ../bin/control.lib

# ADT
INCLUDEPATH += C:/ADT
win32: LIBS += C:/ADT/adt8949.lib

