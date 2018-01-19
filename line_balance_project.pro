#-------------------------------------------------
#
# Project created by QtCreator 2017-07-26T15:35:01
#
#-------------------------------------------------

QT       += core gui
QT       += core gui widgets xlsx

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG -= c++11
QMAKE_CXXFLAGS += -std=c++1z

TARGET = LineBalancingR
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cc\
        lb_main.cc \
    interface_control/ribbon.cc \
    interface_control/ribbon_lb.cc \
    lb_widget.cc \
    stack_panel.cpp \
    station_area.cpp \
    line_label.cpp \
    interface_control/takt_time_dlg.cc \
    data_balance.cpp \
    verification.cc \
    verification_dlg.cc \
    encryption.cc \
    net_utils.cc \
    des.cc

HEADERS  += lb_main.h \
    interface_control/ribbon.h \
    interface_control/ribbon_lb.h \
    lb_widget.h \
    stack_panel.h \
    station_area.h \
    line_label.h \
    interface_control/takt_time_dlg.h \
    data_balance.h \
    verification.h \
    verification_dlg.h \
    encryption.h \
    net_utils.h \
    des.h

FORMS    += lb_main.ui \
    lb_widget.ui \
    verification_dlg.ui

LIBS += -lboost_filesystem
LIBS += -lboost_system
LIBS += -lboost_regex
LIBS += -lboost_thread
LIBS += -lboost_locale
LIBS += -liconv
LIBS += -lwininet
LIBS += -lws2_32
