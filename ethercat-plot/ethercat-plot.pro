#-------------------------------------------------
#
# Project created by QtCreator 2018-08-10T10:34:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = ethercat-plot
TEMPLATE = app

#CONFIG += c++11 console

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix{
  unix:include("modules/SOEM-linux.pri")
  #指定生成的应用程序放置的目录
  unix: DESTDIR += $$PWD/build/linux/bin
  #指定目标文件(obj)的存放目录
  unix: OBJECTS_DIR += $$PWD/build/linux/obj
}

win32{
  win32:DEFINES +=_CRT_SECURE_NO_WARNINGS __STDC_LIMIT_MACROS
  win32:include("modules/SOEM-win.pri")
  win32:include("modules/pthread-win.pri")
  win32:HEADERS += user/c_source/platform/ethercat_compatible_win.h
  #指定生成的应用程序放置的目录
  win32: DESTDIR += $$PWD/build/win32/bin
  #指定目标文件(obj)的存放目录
  win32: OBJECTS_DIR += $$PWD/build/win32/obj
}

#release {
#TARGET = ethercat-plot       #指定生成的应用程序名
#OBJECTS_DIR = ../../obj/ethercat-plot/release #指定目标文件(obj)的存放目录
#}
#debug {
#TARGET = ethercat-plot_d     #指定生成的应用程序名
#OBJECTS_DIR = ../../obj/ethercat-plot/debug #指定目标文件(obj)的存放目录
#}

#指定moc命令将含Q_OBJECT的头文件转换成标准.h文件的存放目录
MOC_DIR = $${OBJECTS_DIR}
#指定uic命令将.ui文件转化成ui_*.h文件的存放的目录
UI_DIR += $${OBJECTS_DIR}


# qcustomplot
include("modules/qcustomplot.pri")

# add console
CONFIG +=console

# user
INCLUDEPATH += .\
    $$PWD/user/c_source/ \
    $$PWD/user/cPlus_source/

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    user/c_source/my_queue.c \
    user/c_source/slaveinfo.c \
    user/c_source/soem_main.c

HEADERS += \
        mainwindow.h \
    user/c_source/my_queue.h \
    user/c_source/soem_main.h

FORMS += \
        mainwindow.ui

