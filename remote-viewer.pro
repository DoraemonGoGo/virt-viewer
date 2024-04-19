#-------------------------------------------------
#
# Project created by QtCreator 2024-04-13T10:46:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = remote-viewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

#QT += widgets
#QT += core gui
QT += gui x11extras
CONFIG += no_keywords
CXXFLAGS += -g

INCLUDEPATH += /usr/include/glib-2.0 /usr/include/glib-2.0/gobject /usr/include/glib-2.0/glib /usr/lib/x86_64-linux-gnu/glib-2.0/include /usr/lib/x86_64-linux-gnu /usr/include/spice-client-glib-2.0 /usr/include/pixman-1 /usr/include/spice-1 ./common ./headers
LIBS += `pkg-config --libs glib-2.0 spice-client-glib-2.0 gobject-2.0 X11`

CONFIG += c++11 link_pkgconfig
PKGCONFIG += glib-2.0 spice-client-glib-2.0 x11 gobject-2.0


SOURCES += \
        main.cpp \
        spicewindow.cpp \
        spice-widget.cpp

HEADERS += \
        headers/spicewindow.h \
        headers/spice-common.h \
        headers/spice-gtk-session.h \
        headers/spice-widget-priv.h \
        headers/spice-widget.h \
    headers/virt-viewer-session.h

FORMS += \
        spicewindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
