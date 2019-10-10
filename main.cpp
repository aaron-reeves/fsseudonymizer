//#include "cmainwindow.h"
//#include <QApplication>

#ifdef CONSOLEAPP
  #include <QtCore>
  #include <ar_general_purpose/csv.h>
  #include <ar_general_purpose/cfilelist.h>
  #include <ar_general_purpose/qcout.h>
  #include <BVDShared/cprocessor.h>
#else
  #include <QApplication>
  #include <QtCore>
  #include <QtGui>
  #include <cmainwindow.h>
#include <BVDShared/cbvdrecord.h>
#endif

#ifdef CONSOLEAPP

/*
 * Sets up the expected command-line arguments
 */
void setupCmdArgs( QCommandLineParser* cmd, QCoreApplication* app ) {
  cmd->setSingleDashWordOptionMode( QCommandLineParser::ParseAsLongOptions );

  QString descr = QString( "%1 %2\n" ).arg( app->applicationName() ).arg( app->applicationVersion() );
  descr.append(
    "Copyright (C) 2015 Epidemiology Research Unit, Scotland's Rural College (SRUC).\n"
    "This is free software, released under the terms of the GNU General Public License.\n"
    "Please see the source or the following URL for copying conditions.\n"
    "<http://epidemiology.sruc.ac.uk/bvdfree>\n"
    "\n"
    // FIXME: Add more description here.
  );

  cmd->setApplicationDescription( descr );
  cmd->addHelpOption();
  cmd->addVersionOption();

  QCommandLineOption cmdInputPath( QStringList() << "i" << "input", "Input file path name.  Required.", "inputFilePath" );
  cmd->addOption( cmdInputPath );
  QCommandLineOption cmdOutputPath( QStringList() << "o" << "output", "Ouput file path name.  Optional, defaults to <input file path name>.bvd.", "outputFilePath" );
  cmd->addOption( cmdOutputPath );
  QCommandLineOption cmdPassPhrase( QStringList() << "p" << "passphrase", "Pass phrase to be used for encryption.  Required.", "passPhrase" );
  cmd->addOption( cmdPassPhrase );
  QCommandLineOption cmdUser( QStringList() << "u" << "username", "Name of user responsible for file processing.  Required.", "userName" );
  cmd->addOption( cmdUser );
  QCommandLineOption cmdEmail( QStringList() << "m" << "email", "Email address of user responsible for file processing.  Required.", "emailAddress" );
  cmd->addOption( cmdEmail );

  QCommandLineOption cmdWriteErrFile( QStringList() << "l" << "logerrors", "Optional.  If present, write a file containing a list of all validation errors encountered." );
  cmd->addOption( cmdWriteErrFile );
  QCommandLineOption cmdErrFilePath( QStringList() << "e" << "errorfile", "Path name for error file generated if any validation errors are encountered, if using --logerrors.  Optional, defaults to <input file path name>.err", "errorFilePath" );
  cmd->addOption( cmdErrFilePath );

  QCommandLineOption cmdSilent( QStringList() << "s" << "silent", "Optional.  If present, do not display error messages while running or upon completion." );
  cmd->addOption( cmdSilent );

  QCommandLineOption cmdNml( QStringList() << "n" << "nml", "Optional.  If present, encrypt the lab test reference number.  This option is used by NML, which includes holding numbers as part of its lab test references." );
  cmd->addOption( cmdNml );

  QCommandLineOption cmdPchs( QStringList() << "p" << "pchs", "Optional.  If present, do not include parish numbers in output.  This option is used by PCHS, which cannot distribute parish numbers." );
  cmd->addOption( cmdPchs );

  #ifdef AR_DEBUG
    QCommandLineOption cmdForce( QStringList() << "f" << "force", "Optional.  Available only during debugging.  If present, bypass some data integrity safeguards." );
    cmd->addOption( cmdForce );
  #endif

  cmd->process( *app );
}


/*
 * Parses command-line arguments
 *
 * No validation is attempted here.  This is done by CProcessor.
 */
QHash<QString, QVariant> processCmdArgs( QCommandLineParser* cmd ) {
  QHash<QString, QVariant> params;

  if( cmd->isSet( "passphrase" ) )
    params.insert( "passphrase", cmd->value( "passphrase" ) );

  if( cmd->isSet( "input" ) )
    params.insert( "inputPath", cmd->value( "input" ) );

  if( cmd->isSet( "username" ) )
    params.insert( "userName", cmd->value( "username" ) );

  if( cmd->isSet( "email" ) )
    params.insert( "userEmail", cmd->value( "email" ) );

  if( cmd->isSet( "output" ) )
    params.insert( "outputPath", cmd->value( "output" ) );

  if( cmd->isSet( "errorfile" ) )
    params.insert( "errPath", cmd->value( "errorfile" ) );

  params.insert( "logerrors", cmd->isSet( "logerrors" ) );
  params.insert( "silent", cmd->isSet( "silent" ) );
  params.insert( "nml", cmd->isSet( "nml" ) );
  params.insert( "pchs", cmd->isSet( "pchs" ) );

  #ifdef AR_DEBUG
    params.insert( "force", cmd->isSet( "force" ) );
  #else
    params.insert( "force", false );
  #endif

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
  #ifdef CONSOLEAPP
    // Set up the core application.
    //-----------------------------
    QCoreApplication app(argc, argv);
    app.setApplicationName( "BVDEncrypt" );
    app.setApplicationVersion( APP_VERSION );

    // Define commandline arguments.
    //------------------------------
    QCommandLineParser* cmd = new QCommandLineParser();
    setupCmdArgs( cmd, &app );

    // Handle commandline arguments.
    //------------------------------
    QHash<QString, QVariant> params = processCmdArgs( cmd );
    CProcessor processor( params, !cmd->isSet( "force" ) );

    // Run the process.
    //-----------------
    if( CProcessor::SUCCESS == processor.result() )
      processor.readAndEncryptData();

    // Display errors to console, if requested.
    //-----------------------------------------
    if( (CProcessor::SUCCESS != processor.result()) && !processor.silent() ) {
      cout << "The following errors were encountered:" << endl;
      cout << processor.errors();
      cout << flush;
    }

    // Clean up and go home.
    //----------------------
    delete cmd;

    return processor.result();
  #else
    QApplication app(argc, argv);
    app.setApplicationName( QStringLiteral("BVDEncrypt") );
    app.setApplicationVersion( QStringLiteral( APP_VERSION ) );
    CMainWindow w;
    w.show();
    return app.exec();
  #endif
}

