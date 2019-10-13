/*
XLPseudonymizer - cmainwindow.h/cpp
------------------------------------
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

#include <qt_widgets/cfileselect.h>
#include <qt_widgets/cmessagedialog.h>
#include <qt_widgets/caboutform.h>

#include "globals.h"

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::CMainWindow) {
  ui->setupUi(this);

  this->setWindowTitle(qApp->applicationName() );

  ui->fsInputFile->setLabel( QStringLiteral("Input file") );
  ui->fsInputFile->setMode( CFileSelect::ModeExistingFile | CFileSelect::ModeOpenFile );
  ui->fsInputFile->setStyleSheet( this->styleSheet() );
  ui->fsInputFile->setFilter( QStringLiteral("CSV (comma-delimited) files (*.csv);; Microsoft Excel spreadsheets (*.xls, *.xlsx);; All files (*.*)") );

  ui->fsOutputFile->setLabel( QStringLiteral("Output file") );
  ui->fsOutputFile->setMode( CFileSelect::ModeAnyFile | CFileSelect::ModeSaveFile );
  ui->fsOutputFile->setStyleSheet( this->styleSheet() );
  ui->fsOutputFile->setFilter( QStringLiteral("CSV (comma-delimited) files (*.csv);; Microsoft Excel spreadsheets (*.xlsx);; All files (*.*)") );

  ui->fsErrorFile->setLabel( QStringLiteral("Error log file") );
  ui->fsErrorFile->setMode( CFileSelect::ModeAnyFile | CFileSelect::ModeSaveFile );
  ui->fsErrorFile->setStyleSheet( this->styleSheet() );
  ui->fsErrorFile->setFilter( QStringLiteral("Error log (plain-text) files (*.txt);; All files (*.*)") );
  ui->fsErrorFile->setEnabled( false );

  ui->btnProcess->setEnabled( false );

  ui->acnAbout->setText( QStringLiteral("%About %1").arg( CGlobals::AppName() ) );
  ui->acnAbout->setIconText( QStringLiteral("About %1").arg( CGlobals::AppName() ) );
  ui->acnAbout->setToolTip( QStringLiteral("About %1").arg( CGlobals::AppName() ) );

  connect( ui->lePassPhrase, SIGNAL( textChanged( QString ) ), this, SLOT( updateParams( QString ) ) );
  connect( ui->leUserName, SIGNAL( textChanged( QString ) ), this, SLOT( updateParams( QString ) ) );
  connect( ui->leEmail, SIGNAL( textChanged( QString ) ), this, SLOT( updateParams( QString ) ) );
  connect( ui->fsInputFile, SIGNAL( textChanged( QString ) ), this, SLOT( updateParams( QString ) ) );
  connect( ui->fsOutputFile, SIGNAL( textChanged( QString ) ), this, SLOT( updateParams( QString ) ) );
  connect( ui->fsErrorFile, SIGNAL( textChanged( QString ) ), this, SLOT( updateParams( QString ) ) );

  connect( ui->lePassPhrase, SIGNAL( textEdited( QString ) ), this, SLOT( updateParams( QString ) ) );
  connect( ui->leUserName, SIGNAL( textEdited( QString ) ), this, SLOT( updateParams( QString ) ) );
  connect( ui->leEmail, SIGNAL( textEdited( QString ) ), this, SLOT( updateParams( QString ) ) );

  connect( ui->cbxWriteErrorLog, SIGNAL( clicked() ), this, SLOT( updateParams() ) );

  connect( ui->btnProcess, SIGNAL( clicked() ), this, SLOT( process() ) );

  connect( ui->acnQuit, SIGNAL( triggered() ), qApp, SLOT( quit() ) );
  connect( ui->acnAbout, SIGNAL( triggered() ), this, SLOT( about() ) );

  initializeParams();
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

  qDebug() << "UPDATE PARAMS";

  bool emptyStr = false;

  if( 0 < ui->lePassPhrase->text().trimmed().length() )
    _params.insert( QStringLiteral("passphrase"), ui->lePassPhrase->text().trimmed() );
  else {
    _params.insert( QStringLiteral("passphrase"), QString() );
    emptyStr = true;
  }

  //qDebug() << "Input path:" << ui->fsInputFile->pathName();
  if( 0 < ui->fsInputFile->pathName().length() )
    _params.insert( QStringLiteral("input"), ui->fsInputFile->pathName() );
  else {
    _params.insert( QStringLiteral("input"), QString() );
    emptyStr = true;
  }

  //qDebug() << "Output path:" << ui->fsOutputFile->pathName();
  if( 0 < ui->fsOutputFile->pathName().length() )
    _params.insert( QStringLiteral("output"), ui->fsOutputFile->pathName() );
  else {
    _params.insert( QStringLiteral("output"), QString() );
    emptyStr = true;
  }

  if( 0 < ui->leUserName->text().trimmed().length() )
    _params.insert( QStringLiteral("username"), ui->leUserName->text().trimmed() );
  else {
    _params.insert( QStringLiteral("username"), QString() );
    emptyStr = true;
  }

  if( 0 < ui->leEmail->text().trimmed().length() )
    _params.insert( QStringLiteral("email"), ui->leEmail->text().trimmed() );
  else {
    _params.insert( QStringLiteral("email"), QString() );
    emptyStr = true;
  }

//  if( ui->cbxWriteErrorLog->isChecked() ) {
//    _params.insert( QStringLiteral("logerrors"), true );
//    ui->fsErrorFile->setEnabled( true );

//    if( 0 < ui->fsErrorFile->pathName().length() )
//      _params.insert( QStringLiteral("errPath"), ui->fsErrorFile->pathName() );
//    else {
//      _params.insert( QStringLiteral("errPath"), QString() );
//      emptyStr = true;
//    }
//  }
//  else {
//    _params.insert( QStringLiteral("logerrors"), false );
//    ui->fsErrorFile->setEnabled( false );
//  }

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

//  if( _params.value( QStringLiteral("logerrors") ).toBool() && QFile::exists( _params.value( QStringLiteral("errPath") ).toString() ) ) {
//    int reply = QMessageBox::question(
//      this,
//      QStringLiteral("Overwrite err log file?"),
//      QStringLiteral("An error log file with a matching name already exists in the selected folder.\n\nProceed and overwrite existing file?")
//    );

//    if( !( QMessageBox::Yes == reply ) )
//      return;
//  }


  this->setControlsEnabled( this, false );

  CProcessor processor( _params );
  QProgressDialog progress( this );
  connect( &processor, SIGNAL( stageStarted( const QString& ) ), &progress, SLOT( setLabelText( const QString& ) ) );
  connect( &processor, SIGNAL( setStageSteps( const int ) ), &progress, SLOT( setMaximum( const int ) ) );
  connect( &processor, SIGNAL( setStageStepComplete( const int ) ), &progress, SLOT( setValue( const int ) ) );

  int result = processor.result();

  if( ReturnCode::SUCCESS == result ) {
    progress.show();
    result = ( result | processor.readRules() );
    progress.setValue( progress.maximum() );
  }

  if( ReturnCode::SUCCESS == result ) {
    result = ( result | processor.readData() );
    progress.setValue( progress.maximum() );
  }

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
  }
  else {
    CMessageDialog dlg(
      QStringLiteral("Could not process file"),
      QStringLiteral("The following problems were encountered:"),
      QMessageBox::Warning,
      this
    );
    dlg.setText( processor.errorMessages() );
    dlg.setWindowModality( Qt::WindowModal );
    dlg.show();
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

  ui->fsErrorFile->setEnabled( ui->cbxWriteErrorLog->isChecked() );
}


void CMainWindow::about() {
  CAboutForm frm( this );

  frm.setPixmap( QStringLiteral(":/images/sruc-small-xparentBackground.png") );
  frm.setEmailAddress( CGlobals::EmailAddress() );
  frm.setBriefDescr( CGlobals::AppDescr() );
  frm.setCopyright( CGlobals::CopyrightInfo() );
  frm.setAuthor( CGlobals::Author() );

  frm.exec();
}

