QT       += gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG(debug, debug|release): TARGET = QFlexd
else:CONFIG(release, debug|release): TARGET = QFlex
TEMPLATE  = lib
DESTDIR   = ../../bin

DEFINES  += QT_FLEX_BUILD
DEFINES  += QT_DEPRECATED_WARNINGS

SOURCES  += \
            QtDockGuider.cpp \
            QtDockSide.cpp \
            QtDockSite.cpp \
            QtDockWidget.cpp \
            QtFlexHelper.cpp \
            QtFlexManager.cpp \
            QtFlexStyle.cpp \
            QtFlexWidget.cpp

HEADERS  += \
            QtDockGuider.h \
            QtDockSide.h \
            QtDockSite.h \
            QtDockWidget.h \
            QtFlexHelper.h \
            QtFlexManager.h \
            QtFlexStyle.h \
            QtFlexWidget.h

RESOURCES += \
            QtFlex.qrc

unix {
    target.path = /usr/lib
    INSTALLS += target
}
