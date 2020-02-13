## Fsseudonymizer.pro
## -------------------
## Begin: 2019/10/12
## Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
## ---------------------------------------------------
## Copyright (C) 2019 - 2020 Scotland's Rural College (SRUC)
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
#CONFIG += gui_application
## !!!-----------------------


## !!!--------------------------------------------------------------------------------------------------
# Definitions for the application name and version
## !!!--------------------------------------------------------------------------------------------------
VERSION = "1.1.1.20200213"  ## !!! Remember when updating version numbers to also update the RC file !!!
VERSION_SHORT = 1.1.1       ## !!! Remember when updating version numbers to also update the RC file !!!
DATE = "13-Feb-2020"        ## !!! Remember when updating version numbers to also update the RC file !!!

RC_FILE = Fsseudonymizer.rc ## Contains the application icon
## !!!--------------------------------------------------------------------------------------------------

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
      ../submodules/libmagic/lib64/magic1.lib \
      ../submodules/libxls/lib64/libxlsreader.a

    INCLUDEPATH += \
      ../submodules/libmagic/include64 \
      ../submodules/libxls/include
  } else {
    DEFINES += MINGW32

    LIBS += \
      ../submodules/libmagic/lib32/magic.lib \
      ../submodules/libxls/lib32/libxlsreader.a

    INCLUDEPATH += \
      ../submodules/libmagic/include32 \
      ../submodules/libxls/include
  }
} else {
  LIBS += \
    /usr/lib/x86_64-linux-gnu/libmagic.so \
    /usr/local/libxls/lib/libxlsreader.so

  INCLUDEPATH += \
    /usr/local/libxls/include
}


INCLUDEPATH += ../submodules


SOURCES += \
  main.cpp\
  ../submodules/ar_general_purpose/cfilelist.cpp \
  ../submodules/ar_general_purpose/returncodes.cpp \
  ../submodules/ar_general_purpose/csv.cpp \
  ../submodules/ar_general_purpose/strutils.cpp \
  ../submodules/ar_general_purpose/qcout.cpp \
  ../submodules/ar_general_purpose/log.cpp \
  ../submodules/ar_general_purpose/filemagic.cpp \
  ../submodules/ar_general_purpose/cspreadsheetarray.cpp \
  ../submodules/ar_general_purpose/debugutils.cpp \
  cpseudonymizerrules.cpp \
  cprocessor.cpp

HEADERS += \
  globals.h \
  ../submodules/ar_general_purpose/cfilelist.h \
  ../submodules/ar_general_purpose/returncodes.h \
  ../submodules/ar_general_purpose/csv.h \
  ../submodules/ar_general_purpose/strutils.h \
  ../submodules/ar_general_purpose/qcout.h \
  ../submodules/ar_general_purpose/log.h \
  ../submodules/ar_general_purpose/filemagic.h \
  ../submodules/ar_general_purpose/cspreadsheetarray.h \
  ../submodules/ar_general_purpose/xlcsv.h \
  ../submodules/ar_general_purpose/ctwodarray.h \
  ../submodules/ar_general_purpose/debugutils.h \
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


console {
  # Nothing to do in here.
} else {
  SOURCES += \
    gui/cmainwindow.cpp \
    ../submodules/qt_widgets/cfileselect.cpp \
    ../submodules/qt_widgets/cmessagedialog.cpp \
    ../submodules/qt_widgets/caboutform.cpp \
    ../submodules/qt_widgets/qclickablelabel.cpp

  HEADERS += \
    gui/cmainwindow.h \
    ../submodules/qt_widgets/cfileselect.h \
    ../submodules/qt_widgets/cmessagedialog.h \
    ../submodules/qt_widgets/caboutform.h \
    ../submodules/qt_widgets/qclickablelabel.h

  FORMS += \
    gui/cmainwindow.ui \
    ../submodules/qt_widgets/cfileselect.ui \
    ../submodules/qt_widgets/cmessagedialog.ui \
    ../submodules/qt_widgets/caboutform.ui
}


RESOURCES += \
  images/images.qrc \
  ../sampleRules/rules.qrc \
  ../submodules/qt_widgets/qt_widgets.qrc





