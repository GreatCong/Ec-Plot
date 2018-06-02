#-------------------------------------------------
#
# Project created by QtCreator 2018-05-31T08:55:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ethercat-plot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot/qcustomplot.cpp \
    soem_src/osal/linux/osal.c \
    soem_src/oshw/linux/nicdrv.c \
    soem_src/oshw/linux/oshw.c \
    soem_src/soem/ethercatbase.c \
    soem_src/soem/ethercatcoe.c \
    soem_src/soem/ethercatconfig.c \
    soem_src/soem/ethercatdc.c \
    soem_src/soem/ethercatfoe.c \
    soem_src/soem/ethercatmain.c \
    soem_src/soem/ethercatprint.c \
    soem_src/soem/ethercatsoe.c \
    user/c_source/soem_main.c \
    user/c_source/my_queue.c \
    user/c_source/slaveinfo.c

HEADERS  += mainwindow.h \
    qcustomplot/qcustomplot.h \
    soem_src/osal/linux/osal_defs.h \
    soem_src/osal/osal.h \
    soem_src/oshw/linux/nicdrv.h \
    soem_src/oshw/linux/oshw.h \
    soem_src/soem/ethercat.h \
    soem_src/soem/ethercatbase.h \
    soem_src/soem/ethercatcoe.h \
    soem_src/soem/ethercatconfig.h \
    soem_src/soem/ethercatconfiglist.h \
    soem_src/soem/ethercatdc.h \
    soem_src/soem/ethercatfoe.h \
    soem_src/soem/ethercatmain.h \
    soem_src/soem/ethercatprint.h \
    soem_src/soem/ethercatsoe.h \
    soem_src/soem/ethercattype.h \
    user/c_source/soem_main.h \
    user/c_source/my_queue.h

FORMS    += mainwindow.ui

#add console
CONFIG +=console

#add includepath
#soem
INCLUDEPATH += \
    soem_src/osal/ \
    soem_src/osal/linux/ \
    soem_src/oshw/linux/ \
    soem_src/soem/

#qcustomplot
INCLUDEPATH += \
    qcustomplot/

#user
INCLUDEPATH += \
    user/c_source/ \
    user/cPlus_source/
