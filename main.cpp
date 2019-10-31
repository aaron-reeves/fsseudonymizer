/*
XLPseudonymizer - main.cpp
--------------------------
Begin: 2019/10/12
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2019 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/


#ifdef CONSOLE_APP
  #include <QCoreApplication>
  #include <QtCore>
  #include <ar_general_purpose/csv.h>
  #include <ar_general_purpose/cfilelist.h>
  #include <ar_general_purpose/qcout.h>
  #include <ar_general_purpose/log.h>
  #include <ar_general_purpose/returncodes.h>
#else
  #include <QApplication>
  #include <QtCore>
  #include <QtGui>
  #include "gui/cmainwindow.h"
#endif

#include "globals.h"
#include "cprocessor.h"

#ifdef CONSOLE_APP
/*
 * Sets up the expected command-line arguments
 */
void setupCmdArgs( QCommandLineParser* cmd, QCoreApplication* app ) {
  cmd->setSingleDashWordOptionMode( QCommandLineParser::ParseAsLongOptions );

  QString descr = QStringLiteral( "%1 %2\n" ).arg( app->applicationName(), app->applicationVersion() );
  descr.append(
    "Copyright (C) 2019 Scotland's Rural College (SRUC).\n"
    "This is free software, released under the terms of the GNU General Public License.\n"
    "Please see the source or the following URL for copying conditions.\n"
    "<http://epidemiology.sruc.ac.uk/bvdfree>\n"
    "\n"
    // FIXME: Add more description here.
  );

  cmd->setApplicationDescription( descr );
  cmd->addHelpOption();
  cmd->addVersionOption();

  QCommandLineOption cmdInputPath(
    QStringList() << QStringLiteral("i") << QStringLiteral("input"),
    QStringLiteral("Input (data) file path. Required."),
    QStringLiteral("inputFilePath")
  );
  cmd->addOption( cmdInputPath );
  QCommandLineOption cmdConfigPath(
    QStringList() << QStringLiteral("r") << QStringLiteral("rules"),
    QStringLiteral("Rules file path. Required."),
    QStringLiteral("rulesFilePath")
  );
  cmd->addOption( cmdConfigPath );
  QCommandLineOption cmdOutputPath(
    QStringList() << QStringLiteral("o") << QStringLiteral("output"),
    QStringLiteral("Ouput file path. Optional, defaults to <inputFilePath>-pseudonymized.csv|xlsx."),
    QStringLiteral("outputFilePath")
  );
  cmd->addOption( cmdOutputPath );
  QCommandLineOption cmdPassPhrase(
    QStringList() << QStringLiteral("p") << QStringLiteral("passphrase"),
    QStringLiteral("Pass phrase to be used for encryption. Required."),
    QStringLiteral("passPhrase")
  );
  cmd->addOption( cmdPassPhrase );
  QCommandLineOption cmdUser(
    QStringList() << QStringLiteral("u") << QStringLiteral("username"),
    QStringLiteral("Name of user responsible for file processing. Optional."),
    QStringLiteral("userName")
  );
  cmd->addOption( cmdUser );
  QCommandLineOption cmdEmail(
    QStringList() << QStringLiteral("m") << QStringLiteral("email"),
    QStringLiteral("Email address of user responsible for file processing. Optional."),
    QStringLiteral("emailAddress")
  );
  cmd->addOption( cmdEmail );

//  QCommandLineOption cmdWriteErrFile( QStringList() << "l" << "logerrors", "Optional.  If present, write a file containing a list of all validation errors encountered." );
//  cmd->addOption( cmdWriteErrFile );
//  QCommandLineOption cmdErrFilePath( QStringList() << "e" << "errorfile", "Path name for error file generated if any validation errors are encountered, if using --logerrors.  Optional, defaults to <input file path name>.err", "errorFilePath" );
//  cmd->addOption( cmdErrFilePath );

//  QCommandLineOption cmdSilent( QStringList() << "s" << "silent", "Optional.  If present, do not display error messages while running or upon completion." );
//  cmd->addOption( cmdSilent );

  cmd->process( *app );
}


/*
 * Parses command-line arguments, and performs some basic validation
 */
QHash<QString, QString> processCmdArgs( QCommandLineParser* cmd, bool& error, QString& errMsg ) {
  QHash<QString, QString> params;

  error = false;
  errMsg = QString();

  if( cmd->isSet( QStringLiteral("input") ) )
    params.insert( QStringLiteral("input"), cmd->value( QStringLiteral("input") ) );
  else {
    error = true;
    errMsg.append( "Input file path is missing.\n" );
  }

  if( cmd->isSet( QStringLiteral("rules") ) )
    params.insert( QStringLiteral("rules"), cmd->value( QStringLiteral("rules") ) );
  else {
    error = true;
    errMsg.append( "Configuration file path is missing.\n" );
  }

  if( cmd->isSet( QStringLiteral("passphrase") ) ) {
    params.insert( QStringLiteral("passphrase"), cmd->value( QStringLiteral("passphrase") ) );
  }
  else {
    error = true;
    errMsg.append( "Pass phrase is missing.\n" );
  }

  if( cmd->isSet( QStringLiteral("username") ) )
    params.insert( QStringLiteral("username"), cmd->value( QStringLiteral("username") ) );

  if( cmd->isSet( QStringLiteral("email") ) )
    params.insert( QStringLiteral("email"), cmd->value( QStringLiteral("email") ) );

  if( cmd->isSet( QStringLiteral("output") ) ) {
    params.insert( QStringLiteral("output"), cmd->value( QStringLiteral("output") ) );
  }

//  if( cmd->isSet( "errorfile" ) )
//    params.insert( "errPath", cmd->value( "errorfile" ) );

//  params.insert( "logerrors", cmd->isSet( "logerrors" ) );
//  params.insert( "silent", cmd->isSet( "silent" ) );

  return params;
}
#endif


/*
 * The main loop
 *
 * All real work is done by instances of CProcessor.
 * For the command-line version, an instance is set up directly.
 * For the GUI version, CMainWindow, which inherits CProcessor, is used.
 */
int main(int argc, char *argv[]) {
  #ifdef CONSOLE_APP
    int result = ReturnCode::SUCCESS;

    // Set up the core application
    //----------------------------
    QCoreApplication app(argc, argv);
    app.setApplicationName( QStringLiteral( APP_NAME ) );
    app.setApplicationVersion( QStringLiteral( APP_VERSION ) );

    // Set up the application log
    //---------------------------
    appLog.setAutoTruncate( false );
    appLog.setConsoleEcho( true );
    appLog.setUseSpacerLine( false );
    appLog.setWindowsFriendly( true );
    appLog.openLog( QStringLiteral("pseudonymizer.log"), LoggingTypical );

    // Define commandline arguments
    //-----------------------------
    QCommandLineParser cmd;
    setupCmdArgs( &cmd, &app );

    // Handle commandline arguments
    //-----------------------------
    bool error;
    QString errorMsg;

    QHash<QString, QString> params = processCmdArgs( &cmd, error, errorMsg );

    if( error ) {
      cout << errorMsg << endl;
      return ReturnCode::BAD_COMMAND;
    }

    // Run the process
    //----------------
    CProcessor processor( params );
    result = ( result | processor.result() );

    if( ReturnCode::SUCCESS == result ) {
      result = ( result | processor.readRules() );
    }

    if( ReturnCode::SUCCESS == result ) {
      result = ( result | processor.readData() );
    }

    if( ReturnCode::SUCCESS == result ) {
      result = ( result | processor.process() );
    }

    // Write output file
    //------------------
    if( ReturnCode::SUCCESS == result ) {
      result = ( result | processor.writeOutput() );
    }

    // Display errors to console, if requested
    //----------------------------------------
    if( ReturnCode::SUCCESS != result ) {
      cout << "The following errors were encountered:" << endl;
      foreach( const QString& msg, processor.errorMessages() ) {
        cout << msg << endl;
      }
      cout << flush;
    }

    // Clean up and go home
    //---------------------
    return result;
  #else
    QApplication app(argc, argv);
    app.setApplicationName( CGlobals::AppName() );
    app.setApplicationVersion( CGlobals::AppVersion() );
    app.setOrganizationName( CGlobals::OrgName() );
    app.setOrganizationDomain( CGlobals::OrgDomain() );

    CMainWindow w;
    w.show();
    return app.exec();
  #endif
}

