#-------------------------------------------------
#
# Project created by QtCreator 2019-10-09T11:33:28
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ecanspy3
TEMPLATE = app

INCLUDEPATH += F:\\projects\\cpp_projects\\HostComputerProjects\\CommonTool\\ECanSpy3\\Projects
INCLUDEPATH += F:\\projects\\cpp_projects\\HostComputerProjects\\CommonTool\\ECanSpy3\\Projects\\ecanspy3\\can

CONFIG(debug,debug|release) {
LIBS += F:\\projects\\cpp_projects\\HostComputerProjects\\CommonTool\\ECanSpy3\\Projects\\build-dbc4cpp-Qt4_8_7_Win32-Debug\\debug\\libdbc4cpp.a
} else {
LIBS += F:\\projects\\cpp_projects\\HostComputerProjects\\CommonTool\\ECanSpy3\\Projects\\build-dbc4cpp-Qt4_8_7_Win32-Release\\release\\libdbc4cpp.a
}


LIBS += F:\\projects\\cpp_projects\\HostComputerProjects\\CommonTool\\ECanSpy3\\Projects\\ecanspy3\\can\\ECanVci.lib
LIBS += -lwinmm

QMAKE_CXXFLAGS += -std=c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


RESOURCES += \
    ecanspy3.qrc

RC_FILE += ecanspy3.rc

FORMS += \
    aboutecanspy3.ui \
    mainwindow.ui \
    settings/canconfigdialog.ui \
    settings/themesettingdialog.ui

HEADERS += \
    aboutecanspy3.h \
    diag/diagwidget.h \
    mainwindow.h \
    base/mainwindowbase.h \
    base/titletableviewwidget.h \
    base/titletreeviewwidget.h \
    can/Core.h \
    can/ECanVci.H \
    can/UsbCanUtil.h \
    database/databasewidget.h \
    database/dbctablemodel.h \
    log/logwidget.h \
    receive/filtertablemodel.h \
    receive/filterwidget.h \
    receive/receivemessagefilterproxymodel.h \
    receive/receivemessagetreemodel.h \
    receive/receivemessagewidget.h \
    receive/receivewidget.h \
    send/checkedbuttondelegate.h \
    send/sendmessagewidget.h \
    send/sendwidget.h \
    send/signaltablemodel.h \
    send/signaltableviewphysicalvaluedelegate.h \
    send/signalwidget.h \
    send/txmessagetablemodel.h \
    settings/projectfile.h \
    statistic/statisticswidget.h \
    status/statuswidget.h \
    utils/mmtimer.h \
    utils/scrollingfile.h \
    utils/signalaccessor.h \
    utils/signalaccesstest.h \
    settings/canconfigdialog.h \
    settings/themesettingdialog.h

SOURCES += \
    aboutecanspy3.cpp \
    diag/diagwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    base/mainwindowbase.cpp \
    base/titletableviewwidget.cpp \
    base/titletreeviewwidget.cpp \
    can/Core.cpp \
    can/UsbCanUtil.cpp \
    database/databasewidget.cpp \
    database/dbctablemodel.cpp \
    log/logwidget.cpp \
    receive/filtertablemodel.cpp \
    receive/filterwidget.cpp \
    receive/receivemessagefilterproxymodel.cpp \
    receive/receivemessagetreemodel.cpp \
    receive/receivemessagewidget.cpp \
    receive/receivewidget.cpp \
    send/checkedbuttondelegate.cpp \
    send/sendmessagewidget.cpp \
    send/sendwidget.cpp \
    send/signaltablemodel.cpp \
    send/signaltableviewphysicalvaluedelegate.cpp \
    send/signalwidget.cpp \
    send/txmessagetablemodel.cpp \
    settings/projectfile.cpp \
    statistic/statisticswidget.cpp \
    status/statuswidget.cpp \
    utils/mmtimer.cpp \
    utils/scrollingfile.cpp \
    utils/signalaccessor.cpp \
    utils/signalaccesstest.cpp \
    settings/canconfigdialog.cpp \
    settings/themesettingdialog.cpp
