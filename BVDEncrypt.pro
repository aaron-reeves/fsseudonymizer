#-------------------------------------------------
#
# Project created by QtCreator 2015-02-17T13:54:06
#
#-------------------------------------------------

#CONFIG += console

VERSION = "1.5.0.20151105"
DATE = "5-Nov-2015"
DEFINES += \
  APP_VERSION=\\\"$$VERSION\\\" \
  APP_DATE=\\\"$$DATE\\\"

QT += core

console {
  TARGET = BVDEncryptc
  DEFINES += CONSOLEAPP
} else {
  TARGET = BVDEncrypt
  QT += gui
  greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
}

TEMPLATE = app

Debug {
  DEFINES += AR_DEBUG
} else {
  DEFINES += AR_RELEASE
}

# Contains the application icon
RC_FILE = bvdencrypt.rc

INCLUDEPATH += \
  C:/libs/C_libs/filemagic-5.03/include \
  sharedCode \
  sharedCode/qt_widgets

LIBS += \
  C:/libs/C_libs/filemagic-5.03/lib/magic.lib

SOURCES += \
  main.cpp\
  sharedCode/BVDShared/cbvddata.cpp \
  sharedCode/ar_general_purpose/csv.cpp \
  sharedCode/ar_general_purpose/strutils.cpp \
  sharedCode/epic_general_purpose/cph.cpp \
  sharedCode/epic_general_purpose/epicutils.cpp \
  sharedCode/ar_general_purpose/cfilelist.cpp \
  sharedCode/epic_general_purpose/iso3166.cpp \
  sharedCode/ar_general_purpose/qcout.cpp \
  sharedCode/BVDShared/cbvdfield.cpp \
  sharedCode/BVDShared/cbvdrecord.cpp \
  sharedCode/BVDShared/cprocessor.cpp \
  sharedCode/BVDShared/cerror.cpp \
  sharedCode/ar_general_purpose/log.cpp \
  sharedCode/ar_general_purpose/filemagic.cpp \
  sharedCode/epic_general_purpose/cpostcode.cpp


HEADERS += \
  sharedCode/BVDShared/cbvddata.h \
  sharedCode/ar_general_purpose/csv.h \
  sharedCode/ar_general_purpose/strutils.h \
  sharedCode/epic_general_purpose/cph.h \
  sharedCode/epic_general_purpose/epicutils.h \
  sharedCode/ar_general_purpose/cfilelist.h \
  sharedCode/epic_general_purpose/iso3166.h \
  sharedCode/ar_general_purpose/qcout.h \
  sharedCode/BVDShared/cbvdfield.h \
  sharedCode/BVDShared/cbvdrecord.h \
  sharedCode/BVDShared/cprocessor.h \
  sharedCode/BVDShared/cerror.h \
  sharedCode/ar_general_purpose/log.h \
  sharedCode/ar_general_purpose/filemagic.h \
  sharedCode/epic_general_purpose/cpostcode.h


console {
  # Nothing to do in here.
} else {
  SOURCES += \
      cmainwindow.cpp \
    sharedCode/qt_widgets/cfileselect.cpp \
    sharedCode/qt_widgets/cmessagedialog.cpp \
    caboutdialog.cpp

  HEADERS += \
    cmainwindow.h \
    sharedCode/qt_widgets/cfileselect.h \
    sharedCode/qt_widgets/cmessagedialog.h \
    caboutdialog.h

  FORMS += \
    cmainwindow.ui \
    sharedCode/qt_widgets/cfileselect.ui \
    sharedCode/qt_widgets/cmessagedialog.ui \
    caboutdialog.ui
}

RESOURCES += \
    bvdencrypt.qrc




