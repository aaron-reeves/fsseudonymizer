/*
Fsseudonymizer - cmainwindow.h/cpp
-----------------------------------
Begin: 2019/10/13
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2019 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cmainwindow.h"
#include "ui_cmainwindow.h"

#include <QMessageBox>

#include <ar_general_purpose/filemagic.h>
#include <ar_general_purpose/returncodes.h>
#include <ar_general_purpose/cfilelist.h>
#include <ar_general_purpose/log.h>

#include <qt_widgets/cfileselect.h>
#include <qt_widgets/cmessagedialog.h>
#include <qt_widgets/caboutform.h>

#include "globals.h"

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::CMainWindow) {
  //appLog.openLog( QStringLiteral("pseudonymizer.log"), LoggingTypical );

  initializeParams();

  checkResourceForRules();

  ui->setupUi(this);

  QString windowTitle = QStringLiteral( "%1 %2" ).arg( CGlobals::AppName(), CGlobals::AppVersionShort() );

  if( !CGlobals::Edition().isEmpty() ) {
    windowTitle.append( QStringLiteral( " (%1 Edition)").arg( CGlobals::Edition() ) );
  }

  this->setWindowTitle( windowTitle );

  this->removeToolBar( ui->mainToolBar );

  if( !_useRulesFileFromResource ) {
    ui->fsRulesFile->setLabel( QStringLiteral("Rules file") );
    ui->fsRulesFile->setMode( CFileSelect::ModeExistingFile | CFileSelect::ModeOpenFile );
    ui->fsRulesFile->setStyleSheet( this->styleSheet() );
    ui->fsRulesFile->setFilter( QStringLiteral("CSV (comma-delimited) files (*.csv);; Microsoft Excel spreadsheets (*.xls, *.xlsx);; All files (*.*)") );
  }
  else {
    int currentHeight = ui->fsRulesFile->height();
    ui->fsRulesFile->setGeometry( ui->fsRulesFile->x(), ui->fsRulesFile->y(), ui->fsRulesFile->width(), 0 );
    ui->fsRulesFile->setVisible( false );
    this->setMinimumHeight( this->minimumHeight() - currentHeight );
    this->setGeometry( this->x(), this->y(), this->width(), this->height() - currentHeight );
  }

  ui->fsInputFile->setLabel( QStringLiteral("Input file") );
  ui->fsInputFile->setMode( CFileSelect::ModeExistingFile | CFileSelect::ModeOpenFile );
  ui->fsInputFile->setStyleSheet( this->styleSheet() );
  ui->fsInputFile->setFilter( QStringLiteral("CSV (comma-delimited) files (*.csv);; Microsoft Excel spreadsheets (*.xls, *.xlsx);; All files (*.*)") );

  ui->fsOutputFile->setLabel( QStringLiteral("Output file") );
  ui->fsOutputFile->setMode( CFileSelect::ModeAnyFile | CFileSelect::ModeSaveFile );
  ui->fsOutputFile->setStyleSheet( this->styleSheet() );
  ui->fsOutputFile->setFilter( QStringLiteral("CSV (comma-delimited) files (*.csv);; Microsoft Excel spreadsheets (*.xlsx);; All files (*.*)") );

  ui->btnProcess->setEnabled( false );

  ui->acnAbout->setText( QStringLiteral("&About %1").arg( CGlobals::AppName() ) );
  ui->acnAbout->setIconText( QStringLiteral("About %1").arg( CGlobals::AppName() ) );
  ui->acnAbout->setToolTip( QStringLiteral("About %1").arg( CGlobals::AppName() ) );

  connect( ui->lePassPhrase, SIGNAL( textChanged( QString ) ), this, SLOT( updateParams() ) );
  connect( ui->leUserName, SIGNAL( textChanged( QString ) ), this, SLOT( updateParams() ) );
  connect( ui->leEmail, SIGNAL( textChanged( QString ) ), this, SLOT( updateParams() ) );
  connect( ui->fsRulesFile, SIGNAL( pathNameChanged( QString ) ), this, SLOT( updateParams() ) );
  connect( ui->fsInputFile, SIGNAL( pathNameChanged( QString ) ), this, SLOT( updateParams() ) );
  connect( ui->fsOutputFile, SIGNAL( pathNameChanged( QString ) ), this, SLOT( updateParams() ) );

  connect( ui->lePassPhrase, SIGNAL( textEdited( QString ) ), this, SLOT( updateParams() ) );
  connect( ui->leUserName, SIGNAL( textEdited( QString ) ), this, SLOT( updateParams() ) );
  connect( ui->leEmail, SIGNAL( textEdited( QString ) ), this, SLOT( updateParams() ) );

  connect( ui->btnProcess, SIGNAL( clicked() ), this, SLOT( process() ) );

  connect( ui->acnQuit, SIGNAL( triggered() ), qApp, SLOT( quit() ) );
  connect( ui->acnAbout, SIGNAL( triggered() ), this, SLOT( about() ) );

  readSettings();
}


void CMainWindow::checkResourceForRules() {
  _resourceOK = true;
  _useRulesFileFromResource = false;

  CFileList fl( QStringLiteral(":/rules"), QStringLiteral("*.xlsx;*.csv;*.xls"), false );

  int matchedFiles = 0;

  #ifdef Q_OS_WIN
    Qt::CaseSensitivity sens = Qt::CaseInsensitive;
  #else
    Qt::CaseSensitivity sens = Qt::CaseSensitive;
  #endif

  foreach( const CPathString& str, fl ) {
    if( 0 == str.compare( QStringLiteral(":/rules/rules.xlsx"), sens ) ) {
      ++matchedFiles;
      _useRulesFileFromResource = true;
      _params.insert( QStringLiteral("rules"), QStringLiteral(":/rules/rules.xlsx") );
    }

    if( 0 == str.compare( QStringLiteral(":/rules/rules.csv"), sens ) ) {
      ++matchedFiles;
      _useRulesFileFromResource = true;
      _params.insert( QStringLiteral("rules"), QStringLiteral(":/rules/rules.csv") );
    }

    if( 0 == str.compare( QStringLiteral(":/rules/rules.xls"), sens ) ) {
      ++matchedFiles;
      _useRulesFileFromResource = true;
      _params.insert( QStringLiteral("rules"), QStringLiteral(":/rules/rules.xls") );
    }
  }

  if( 1 < matchedFiles ) {
    _params.remove( QStringLiteral("rules") );
    _resourceOK = false;
  }
}


void CMainWindow::readSettings() {
  QSettings settings;

  resize(settings.value( QStringLiteral("MainWindow/size"), QSize( this->width(), this->height() ) ).toSize() );
  move( settings.value( QStringLiteral("MainWindow/pos"), QPoint( 200, 200 ) ).toPoint() );

  QString tmp;

  if( !_useRulesFileFromResource ) {
    tmp = settings.value( QStringLiteral("FileLocations/rulesFile"), QString() ).toString();
    if( !tmp.isEmpty() ) {
      if( QFileInfo::exists( tmp ) ) {
        ui->fsRulesFile->setPathName( tmp );
        _params.insert( QStringLiteral("rules"), tmp );
      }
      else if( QFileInfo::exists( CPathString( tmp ).directory() )) {
        ui->fsRulesFile->setDirectory( CPathString( tmp ).directory() );
      }
    }
  }

  tmp = settings.value( QStringLiteral("FileLocations/inputFile"), QString() ).toString();
  if( !tmp.isEmpty() ) {
    if( QFileInfo::exists( tmp ) ) {
      ui->fsInputFile->setPathName( tmp );
      _params.insert( QStringLiteral("input"), tmp );
    }
    else if( QFileInfo::exists( CPathString( tmp ).directory() )) {
      ui->fsInputFile->setDirectory( CPathString( tmp ).directory() );
    }
  }

  tmp = settings.value( QStringLiteral("FileLocations/outputFile"), QString() ).toString();
  if( !tmp.isEmpty() ) {
    if( QFileInfo::exists( CPathString( tmp ).directory() )) {
      ui->fsOutputFile->setDirectory( CPathString( tmp ).directory() );
    }
  }

  tmp = settings.value( QStringLiteral("EncryptionDetails/passphrase"), QString() ).toString();
  if( !tmp.isEmpty() ) {
    ui->lePassPhrase->setText( tmp );
    _params.insert( QStringLiteral("passphrase"), tmp );
  }

  tmp = settings.value( QStringLiteral("UserInformation/username"), QString() ).toString();
  if( !tmp.isEmpty() ) {
    ui->leUserName->setText( tmp );
    _params.insert( QStringLiteral("username"), tmp );
  }

  tmp = settings.value( QStringLiteral("UserInformation/email"), QString() ).toString();
  if( !tmp.isEmpty() ) {
    ui->leEmail->setText( tmp );
    _params.insert( QStringLiteral("email"), tmp );
  }
}


void CMainWindow::writeSettings() {
  QSettings settings;

  settings.setValue( QStringLiteral("MainWindow/size"), this->size() );
  settings.setValue( QStringLiteral("MainWindow/pos"), this->pos() );

  if( !_params.value(  QStringLiteral("input") ).isEmpty() )
    settings.setValue( QStringLiteral("FileLocations/inputFile"), _params.value( QStringLiteral("input") ) );

  if( !_useRulesFileFromResource && !_params.value(  QStringLiteral("rules") ).isEmpty() )
    settings.setValue( QStringLiteral("FileLocations/rulesFile"), _params.value( QStringLiteral("rules") ) );

  if( !_params.value(  QStringLiteral("output") ).isEmpty() )
    settings.setValue( QStringLiteral("FileLocations/outputFile"), _params.value( QStringLiteral("output") ) );

  if( !_params.value(  QStringLiteral("passphrase") ).isEmpty() )
    settings.setValue( QStringLiteral("EncryptionDetails/passphrase"), _params.value( QStringLiteral("passphrase") ) );

  if( !_params.value(  QStringLiteral("username") ).isEmpty() )
    settings.setValue( QStringLiteral("UserInformation/username"), _params.value( QStringLiteral("username") ) );

  if( !_params.value(  QStringLiteral("email") ).isEmpty() )
    settings.setValue( QStringLiteral("UserInformation/email"), _params.value( QStringLiteral("email") ) );
}


void CMainWindow::showEvent( QShowEvent* event ) {
  QMainWindow::showEvent( event );
  QApplication::processEvents();

  if( _useRulesFileFromResource && !_resourceOK ) {
    this->setControlsEnabled( this, false );
    QTimer::singleShot(50, this, SLOT(resourceWarning()));
  }
}


void CMainWindow::resourceWarning() {
  QMessageBox::critical(
    this,
    QStringLiteral("Problem with rules"),
    QStringLiteral("There is a problem with the built-in rules file for this application.\n\nPlease consult with the developers.")
  );

  this->close();
}


void CMainWindow::closeEvent( QCloseEvent *event ) {
  writeSettings();
  event->accept();
}


CMainWindow::~CMainWindow() {
  delete ui;
}


void CMainWindow::initializeParams() {
  _params.insert( QStringLiteral("input"), QString() );
  _params.insert( QStringLiteral("rules"), QString() );
  _params.insert( QStringLiteral("output"), QString() );
  _params.insert( QStringLiteral("passphrase"), QString() );
  _params.insert( QStringLiteral("username"), QString() );
  _params.insert( QStringLiteral("email"), QString() );
}


void CMainWindow::updateParams() {
  updateParams( QStringLiteral("unused string") );
}


void CMainWindow::updateParams( const QString &unused ) {
  Q_UNUSED( unused )

  bool emptyStr = false;

  // List these in reverse order relative to the way they are displayed on the form.
  // That way, the status bar message will walk users through everything in the right order.

  // Output file name
  //-----------------
  if( ( 0 < ui->fsOutputFile->pathName().length() ) && !QFileInfo( ui->fsOutputFile->pathName() ).isDir() )
    _params.insert( QStringLiteral("output"), ui->fsOutputFile->pathName() );
  else {
    this->statusBar()->showMessage( QStringLiteral("Please specify an output file name.") );
    _params.insert( QStringLiteral("output"), QString() );
    emptyStr = true;
  }

  // Email address
  //--------------
  if( 0 < ui->leEmail->text().trimmed().length() )
    _params.insert( QStringLiteral("email"), ui->leEmail->text().trimmed() );
  else {
    this->statusBar()->showMessage( QStringLiteral("Please specify a valid email address.") );
    _params.insert( QStringLiteral("email"), QString() );
    emptyStr = true;
  }

  // User name
  //----------
  if( 0 < ui->leUserName->text().trimmed().length() )
    _params.insert( QStringLiteral("username"), ui->leUserName->text().trimmed() );
  else {
    this->statusBar()->showMessage( QStringLiteral("Please specify a user name.") );
    _params.insert( QStringLiteral("username"), QString() );
    emptyStr = true;
  }

  // Pass phrase
  //------------
  if( 0 < ui->lePassPhrase->text().trimmed().length() )
    _params.insert( QStringLiteral("passphrase"), ui->lePassPhrase->text().trimmed() );
  else {
    _params.insert( QStringLiteral("passphrase"), QString() );
    emptyStr = true;
  }

  // Rules file name
  //----------------
  if( !_useRulesFileFromResource ) {
    //qDebug() << "Rules path:" << ui->fsRulesFile->pathName();
    if( ( 0 < ui->fsRulesFile->pathName().length() ) && QFileInfo( ui->fsRulesFile->pathName() ).isFile() ) {
      _params.insert( QStringLiteral("rules"), ui->fsRulesFile->pathName() );
    }
    else {
      this->statusBar()->showMessage( QStringLiteral("Please specify a rules file name.") );
      _params.insert( QStringLiteral("rules"), QString() );
      emptyStr = true;
    }
  }

  // Input file name
  //----------------
  //qDebug() << "Input path:" << ui->fsInputFile->pathName();
  if( ( 0 < ui->fsInputFile->pathName().length() ) && QFileInfo( ui->fsInputFile->pathName() ).isFile() )
    _params.insert( QStringLiteral("input"), ui->fsInputFile->pathName() );
  else {
    this->statusBar()->showMessage( QStringLiteral("Please specify an input file name.") );
    _params.insert( QStringLiteral("input"), QString() );
    emptyStr = true;
  }

  if( !emptyStr ) {
    this->statusBar()->showMessage( QStringLiteral("File is ready to process.") );
  }

  ui->btnProcess->setEnabled( !emptyStr );
}


void CMainWindow::process() {
  // Check paths for rules, input, output, and error files
  //------------------------------------------------------
  if( !QFile::exists( _params.value( QStringLiteral("input") ) ) ) {
    QMessageBox::critical(
      this,
      QStringLiteral("Input file does not exist"),
      QStringLiteral("The indicated input file does not exist. Please select a different file.")
    );

    return;
  }

  if( !QFile::exists( _params.value( QStringLiteral("rules") ) ) ) {
    QMessageBox::critical(
      this,
      QStringLiteral("Rules file does not exist"),
      QStringLiteral("The indicated rules file does not exist. Please select a different file.")
    );

    return;
  }

  if( QFile::exists( _params.value( QStringLiteral("output") ) ) ) {
    int reply = QMessageBox::question(
      this,
      QStringLiteral("Overwrite output file?"),
      QStringLiteral("An output file with a matching name already exists in the selected folder.\n\nProceed and overwrite existing file?")
    );

    if( !( QMessageBox::Yes == reply ) )
      return;
  }

  this->setControlsEnabled( this, false );

  CMessageDialog dlgErrors(
    QStringLiteral("Could not process file"),
    QStringLiteral("The following problems were encountered:"),
    QMessageBox::Warning,
    this
  );
  dlgErrors.setWindowModality( Qt::WindowModal );

  connect( &appLog, SIGNAL( messageLogged( QString ) ), &dlgErrors, SLOT( append( QString ) ) );

  this->statusBar()->clearMessage();

  CProcessor processor( _params, _useRulesFileFromResource );

  QProgressDialog progress( this );

  connect( &processor, SIGNAL( stageStarted( const QString& ) ), &progress, SLOT( setLabelText( const QString& ) ) );
  connect( &processor, SIGNAL( setStageSteps( const int ) ), &progress, SLOT( setMaximum( const int ) ) );
  connect( &processor, SIGNAL( setStageStepComplete( const int ) ), &progress, SLOT( setValue( const int ) ) );

  progress.setAutoReset( true );
  progress.setAutoClose( false );

  int result = processor.result();

  if( ReturnCode::SUCCESS == result ) {
    progress.show();
    result = ( result | processor.readRules() );
    progress.setValue( progress.maximum() );
  }
  qDebug() << "readRules:" << result;

  if( ReturnCode::SUCCESS == result ) {
    result = ( result | processor.readData() );
    progress.setValue( progress.maximum() );
  }
  qDebug() << "readData:" << result;

  if( ReturnCode::SUCCESS == result ) {
    result = ( result | processor.process() );
    progress.setValue( progress.maximum() );
  }

  if( ReturnCode::SUCCESS == result ) {
    result = ( result | processor.writeOutput() );
    progress.setValue( progress.maximum() );
  }

  progress.hide();

  if( ReturnCode::SUCCESS == result ) {
    QMessageBox::information( this, QStringLiteral("Success"), QStringLiteral("Data file was successfully processed, and output has been generated.") );
    ui->fsOutputFile->clearPath();
  }
  else {
    dlgErrors.exec();
  }

  this->setControlsEnabled( this, true );

  disconnect( &processor, SIGNAL( stageStarted( const QString& ) ), &progress, SLOT( setLabelText( const QString& ) ) );
  disconnect( &processor, SIGNAL( setStageSteps( const int ) ), &progress, SLOT( setMaximum( const int ) ) );
  disconnect( &processor, SIGNAL( setStageStepComplete( const int ) ), &progress, SLOT( setValue( const int ) ) );
}


void CMainWindow::setControlsEnabled( QWidget* widget, const bool val ) {
  widget->setEnabled( val );
  for( int i = 0; i < widget->children().length(); ++i ) {
    if( widget->children().at(i)->isWidgetType() ) {
      setControlsEnabled( dynamic_cast<QWidget*>(widget->children().at(i)), val );
    }
  }
}


void CMainWindow::about() {
  CAboutForm frm( this );

  frm.setWindowTitle( this->windowTitle() );
  frm.setPixmap( QStringLiteral(":/images/sruc-small-xparentBackground.png") );
  frm.showWebsite();
  frm.setWebsite( CGlobals::Website() );
  frm.setEmailAddress( CGlobals::EmailAddress() );
  frm.setBriefDescr( CGlobals::AppDescr() );
  frm.setCopyright( CGlobals::CopyrightInfo() );
  frm.setAuthor( CGlobals::Author() );

  frm.exec();
}

