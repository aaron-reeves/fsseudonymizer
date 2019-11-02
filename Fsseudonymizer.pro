## Fsseudonymizer.pro
## -------------------
## Begin: 2019/10/12
## Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
## ---------------------------------------------------
## Copyright (C) 2019 Scotland's Rural College (SRUC)
##
## This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
## Public License as published by the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.

CONFIG += MINGW64
#CONFIG += MINGW32

QT += core xlsx

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

CONFIG -= app_bundle
TEMPLATE = app

## !!!-----------------------
## !!! For a GUI application:
## !!!-----------------------
CONFIG += gui_application
## !!!-----------------------


## !!!-------------------------------------------------------------------------------------------------
# Definitions for the application name and version
## !!!-------------------------------------------------------------------------------------------------
VERSION = "1.0.0.20191101" ## !!! Remember when updating version numbers to also update the RC file !!!
VERSION_SHORT = 1.0.0      ## !!! Remember when updating version numbers to also update the RC file !!!
DATE = "01-Nov-2019"       ## !!! Remember when updating version numbers to also update the RC file !!!

RC_FILE = Fsseudonymizer.rc ## Contains the application icon
## !!!-------------------------------------------------------------------------------------------------

DEFINES += \
  APP_VERSION=\\\"$$VERSION\\\" \
  APP_VERSION_SHORT=\\\"$$VERSION_SHORT\\\" \
  APP_DATE=\\\"$$DATE\\\"

gui_application {
  QT += gui
  TARGET = fsseudonymizer
  DEFINES += APP_NAME=\\\"Fsseudonymizer\\\"
} else {
  QT -= gui
  CONFIG += console
  DEFINES += CONSOLE_APP
  TARGET = fsseudonymizerc
  DEFINES += APP_NAME=\\\"FsseudonymizerC\\\"
}


win32 {
  MINGW64 {
    DEFINES += MINGW64

    LIBS += \
      C:/libs/C_libs-x64/libmagicwin64/lib/magic1.lib \
      C:/libs/C_libs-x64/libxls-github-x64/lib/libxlsreader.a

    INCLUDEPATH += \
      C:/libs/C_libs-x64/libmagicwin64/include \
      C:/libs/C_libs-x64/libxls-github-x64/include
  } else {
    DEFINES += MINGW32

    LIBS += \
      C:/libs/C_libs/filemagic-5.03/lib/magic.lib \
      C:/libs/C_libs/libxls-github/lib/libxlsreader.a

    INCLUDEPATH += \
      C:/libs/C_libs/filemagic-5.03/include \
      C:/libs/C_libs/libxls-github/include
  }
} else {
  LIBS += \
    /usr/lib/x86_64-linux-gnu/libmagic.so \
    /usr/local/libxls/lib/libxlsreader.so

  INCLUDEPATH += \
    /usr/local/libxls/include
}


INCLUDEPATH += sharedCode


SOURCES += \
  main.cpp\
  sharedCode/ar_general_purpose/cfilelist.cpp \
  sharedCode/ar_general_purpose/returncodes.cpp \
  sharedCode/ar_general_purpose/csv.cpp \
  sharedCode/ar_general_purpose/strutils.cpp \
  sharedCode/ar_general_purpose/qcout.cpp \
  sharedCode/ar_general_purpose/log.cpp \
  sharedCode/ar_general_purpose/filemagic.cpp \
  sharedCode/ar_general_purpose/cspreadsheetarray.cpp \
  sharedCode/ar_general_purpose/debugutils.cpp \
  cpseudonymizerrules.cpp \
  cprocessor.cpp

HEADERS += \
  globals.h \
  sharedCode/ar_general_purpose/cfilelist.h \
  sharedCode/ar_general_purpose/returncodes.h \
  sharedCode/ar_general_purpose/csv.h \
  sharedCode/ar_general_purpose/strutils.h \
  sharedCode/ar_general_purpose/qcout.h \
  sharedCode/ar_general_purpose/log.h \
  sharedCode/ar_general_purpose/filemagic.h \
  sharedCode/ar_general_purpose/cspreadsheetarray.h \
  sharedCode/ar_general_purpose/xlcsv.h \
  sharedCode/ar_general_purpose/ctwodarray.h \
  sharedCode/ar_general_purpose/debugutils.h \
  cpseudonymizerrules.h \
  cprocessor.h

#-------------------------------------------------------------
# These files can be customized to do more complex processing
# than the main application is capable of supporting.
# Do so in separate branches or forks!
#-------------------------------------------------------------
SOURCES += \
  customprocessing.cpp

HEADERS += \
  customprocessing.h
#-------------------------------------------------------------

#--------------------------------------------------------------------
# These files are used for the EPIC_FSS branch, for pseudonymizing
# data from Food Standards Scotland for use in Topic 3 (surveillance)
#--------------------------------------------------------------------
SOURCES += \
  sharedCode/epic_general_purpose/cph.cpp \
  sharedCode/epic_general_purpose/cpostcode.cpp

HEADERS += \
  sharedCode/epic_general_purpose/cph.h \
  sharedCode/epic_general_purpose/cpostcode.h
#--------------------------------------------------------------------

console {
  # Nothing to do in here.
} else {
  SOURCES += \
    gui/cmainwindow.cpp \
    sharedCode/qt_widgets/cfileselect.cpp \
    sharedCode/qt_widgets/cmessagedialog.cpp \
    sharedCode/qt_widgets/caboutform.cpp \
    sharedCode/qt_widgets/qclickablelabel.cpp

  HEADERS += \
    gui/cmainwindow.h \
    sharedCode/qt_widgets/cfileselect.h \
    sharedCode/qt_widgets/cmessagedialog.h \
    sharedCode/qt_widgets/caboutform.h \
    sharedCode/qt_widgets/qclickablelabel.h

  FORMS += \
    gui/cmainwindow.ui \
    sharedCode/qt_widgets/cfileselect.ui \
    sharedCode/qt_widgets/cmessagedialog.ui \
    sharedCode/qt_widgets/caboutform.ui
}


RESOURCES += \
  images/images.qrc \
  sampleRules/rules.qrc \
  sharedCode/qt_widgets/qt_widgets.qrc





