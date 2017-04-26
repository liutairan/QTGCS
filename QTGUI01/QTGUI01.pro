#-------------------------------------------------
#
# Project created by QtCreator 2017-03-20T22:43:28
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += network
QT       += network widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QTGUI01
TEMPLATE = app
ICON = QTGUI01.icns

SOURCES += main.cpp\
        mainwindow.cpp \
    map.cpp \
    multiwii.cpp \
    quadstates.cpp \
    serialcommunication_usb.cpp \
    serialcommunication_xbee_api.cpp \
    serialcommunication_xbee_at.cpp \
    serialcommunication.cpp \
    dataexchange.cpp \
    inputdialog.cpp \
    filedownloader.cpp \
    qtxb.cpp \
    qtxb/digimeshpacket.cpp \
    qtxb/atcommandqueueparam.cpp \
    qtxb/atcommand.cpp \
    qtxb/txrequest.cpp \
    qtxb/txrequestexplicit.cpp \
    qtxb/atcommandremote.cpp \
    qtxb/atcommandresponse.cpp \
    qtxb/modemstatus.cpp \
    qtxb/transmitstatus.cpp \
    qtxb/rxindicator.cpp \
    qtxb/remotecommandrequest.cpp \
    qtxb/rxindicatorexplicit.cpp \
    qtxb/nodeidentificationindicator.cpp \
    qtxb/remotecommandresponse.cpp

HEADERS  += mainwindow.h \
    map.h \
    multiwii.h \
    quadstates.h \
    serialcommunication_usb.h \
    serialcommunication_xbee_api.h \
    serialcommunication_xbee_at.h \
    serialcommunication.h \
    dataexchange.h \
    inputdialog.h \
    filedownloader.h \
    qtxb.h \
    qtxb/digimeshpacket.h \
    qtxb/atcommandqueueparam.h \
    qtxb/atcommand.h \
    qtxb/txrequest.h \
    qtxb/txrequestexplicit.h \
    qtxb/atcommandremote.h \
    qtxb/atcommandresponse.h \
    qtxb/modemstatus.h \
    qtxb/transmitstatus.h \
    qtxb/rxindicator.h \
    qtxb/remotecommandrequest.h \
    qtxb/rxindicatorexplicit.h \
    qtxb/nodeidentificationindicator.h \
    qtxb/remotecommandresponse.h

FORMS    += mainwindow.ui
