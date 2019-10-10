#include "cmainwindow.h"
#include "ui_cmainwindow.h"

#include <QMessageBox>

#include <ar_general_purpose/filemagic.h>
#include <qt_widgets/cmessagedialog.h>

#include <caboutdialog.h>

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::CMainWindow) {
  ui->setupUi(this);

  this->setWindowTitle(qApp->applicationName() );

  ui->fsInputFile->setLabel( QStringLiteral("Input file") );
  ui->fsInputFile->setMode( CFileSelect::ModeExistingFile | CFileSelect::ModeOpenFile );
  ui->fsInputFile->setFilter( QStringLiteral("CSV (comma-delimited) files (*.csv);; All files (*.*)") );

  ui->fsOutputFile->setLabel( QStringLiteral("Output file") );
  ui->fsOutputFile->setMode( CFileSelect::ModeAnyFile | CFileSelect::ModeSaveFile );
  ui->fsOutputFile->setFilter( QStringLiteral("BVDEncrypt files (*.bvd);; All files (*.*)") );

  ui->fsErrorFile->setLabel( QStringLiteral("Error log file") );
  ui->fsErrorFile->setMode( CFileSelect::ModeAnyFile | CFileSelect::ModeSaveFile );
  ui->fsErrorFile->setFilter( QStringLiteral("Error log (plain-text) files (*.txt);; All files (*.*)") );
  ui->fsErrorFile->setEnabled( false );

  ui->btnEncrypt->setEnabled( false );

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

  connect( ui->cbxNml, SIGNAL( clicked() ), this, SLOT( updateParams() ) );
  connect( ui->cbxPchs, SIGNAL( clicked() ), this, SLOT( updateParams() ) );

  connect( ui->btnEncrypt, SIGNAL( clicked() ), this, SLOT( run() ) );

  connect( ui->action_Quit, SIGNAL( triggered() ), qApp, SLOT( quit() ) );
  connect( ui->action_About_BVDEncrypt, SIGNAL( triggered() ), this, SLOT( about() ) );

  _processor = new CProcessor( false );

  _progress = new QProgressDialog( this );
  connect( _processor, SIGNAL( setProgressMax( int ) ), _progress, SLOT( setMaximum( int ) ) );
  connect( _processor, SIGNAL( setProgressValue( int ) ), _progress, SLOT( setValue( int ) ) );
  connect( _processor, SIGNAL( setProgressLabelText( QString ) ), _progress, SLOT( setLabelText( QString ) ) );
  connect( _progress, SIGNAL( canceled() ), _processor, SLOT( interrupt() ) );
  _progress->setMinimumDuration( 0 );
  _progress->setWindowModality(Qt::WindowModal);

  initializeParams();

  _extraneousQuotesFileExists = false;
  _extraneousQuotesFile = QLatin1String("");

  #ifdef AR_DEBUG
    ui->lePassPhrase->setText( QStringLiteral("my passphrase") );
    ui->leUserName->setText( QStringLiteral("Aaron Reeves") );
    ui->leEmail->setText( QStringLiteral("aaron.reeves@sruc.ac.uk") );
    ui->fsInputFile->setPathName( QStringLiteral("C:/Users/areeves/Documents/ResearchProjects/BVDEngland/sampleData/troubleshooting/BVD example file-nml-20150817.csv") );
    ui->fsOutputFile->setPathName( QStringLiteral("C:/Users/areeves/Documents/ResearchProjects/BVDEngland/sampleData/troubleshooting/nmlTestx.bvd") );
    ui->fsErrorFile->setPathName( QStringLiteral("C:/Users/areeves/Documents/ResearchProjects/BVDEngland/sampleData/troubleshooting/nmlErrorsx.txt") );
    updateParams();
  #endif
}


CMainWindow::~CMainWindow() {
  // _progress is own by this, and is automatically deleted.
  delete _processor;
  delete ui;
}


void CMainWindow::initializeParams() {
  _params.insert( QStringLiteral("logerrors"), false );
  _params.insert( QStringLiteral("inputPath"), QVariant( QVariant::String ) );
  _params.insert( QStringLiteral("outputPath"), QVariant( QVariant::String ) );
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
    _params.insert( QStringLiteral("passphrase"), QVariant( QVariant::String ) );
    emptyStr = true;
  }

  //qDebug() << "Input path:" << ui->fsInputFile->pathName();
  if( 0 < ui->fsInputFile->pathName().length() )
    _params.insert( QStringLiteral("inputPath"), ui->fsInputFile->pathName() );
  else {
    _params.insert( QStringLiteral("inputPath"), QVariant( QVariant::String ) );
    emptyStr = true;
  }

  //qDebug() << "Output path:" << ui->fsOutputFile->pathName();
  if( 0 < ui->fsOutputFile->pathName().length() )
    _params.insert( QStringLiteral("outputPath"), ui->fsOutputFile->pathName() );
  else {
    _params.insert( QStringLiteral("outputPath"), QVariant( QVariant::String ) );
    emptyStr = true;
  }

  if( 0 < ui->leUserName->text().trimmed().length() )
    _params.insert( QStringLiteral("userName"), ui->leUserName->text().trimmed() );
  else {
    _params.insert( QStringLiteral("userName"), QVariant( QVariant::String ) );
    emptyStr = true;
  }

  if( 0 < ui->leEmail->text().trimmed().length() )
    _params.insert( QStringLiteral("userEmail"), ui->leEmail->text().trimmed() );
  else {
    _params.insert( QStringLiteral("userEmail"), QVariant( QVariant::String ) );
    emptyStr = true;
  }

  if( ui->cbxWriteErrorLog->isChecked() ) {
    _params.insert( QStringLiteral("logerrors"), true );
    ui->fsErrorFile->setEnabled( true );

    if( 0 < ui->fsErrorFile->pathName().length() )
      _params.insert( QStringLiteral("errPath"), ui->fsErrorFile->pathName() );
    else {
      _params.insert( QStringLiteral("errPath"), QVariant( QVariant::String ) );
      emptyStr = true;
    }
  }
  else {
    _params.insert( QStringLiteral("logerrors"), false );
    ui->fsErrorFile->setEnabled( false );
  }

  _params.insert( QStringLiteral("nml"), ui->cbxNml->isChecked() );

  _params.insert( QStringLiteral("pchs"), ui->cbxPchs->isChecked() );

  _processor->setParams( _params );

  ui->btnEncrypt->setEnabled( !emptyStr );
}


bool CMainWindow::isTextFile() {
  bool result, error;
  QString fileTypeInfo;
  result = magicIsAsciiTextFile( _params.value( QStringLiteral("inputPath") ).toString(), &error, &fileTypeInfo );

  if( error ) {
    QMessageBox::critical(
      this,
      QStringLiteral("Cannot determine file type"),
      QString(
        "The type of the selected input file could not be determined."
        "  This may indicate a problem with your installation of BVDEncrypt."
        "  If problems persist, please reinstall BVDEncrypt, or contact the developers for further assistance."
      )
    );

    result = false;
  }
  else if( !result ) {
    int reply = QMessageBox::critical(
      this,
      QStringLiteral("Incorrect file type"),
      QString(
        "The selected input file does not appear to be a plain-text, comma-delimited (CSV) file."
        "  The file type is reported to be \"%1\"."
        "\n\nPlease check the file format.  If you are certain that the file is the right type, you can try to proceed anyway."
        "\n\nContinue?"
      ).arg( fileTypeInfo ),
      QMessageBox::Yes | QMessageBox::No
    );

    result = ( QMessageBox::Yes == reply );
  }

  return result;
}


QString CMainWindow::processExtraneousQuotesLine( const QString &line ) {
  QString result = line;
  if( ( '"' == result.at(0) ) && ( '"' == result.at(result.length() - 1) ) )
    result = result.mid( 1, result.length() - 2 );

  qDebug() << result;

  return result;
}


bool CMainWindow::processExtraneousQuotesFile() {
  qDebug() << "-- BEGIN CMainWindow::processExtraneousQuotesFile()";

  // oif is originalInputFile
  // eqf is extraneousQuotesFile
  bool result;

  QFile oif( _params.value( QStringLiteral("inputPath") ).toString() );
  if( !oif.open( QIODevice::ReadOnly | QIODevice::Text ) )
    result = false;
  else {
    QTemporaryFile eqf;
    eqf.setAutoRemove( false );
    if( !eqf.open() ) {
      oif.close();
      eqf.remove();
      result = false;
    }
    else {
      QTextStream eqft( &eqf );
      QTextStream oift( &oif );
      while( !oift.atEnd() ) {
        QString line = processExtraneousQuotesLine( oift.readLine() );
        eqft << line << endl;
      }
      oif.close();
      eqf.close();

      _extraneousQuotesFile = eqf.fileName();
      qDebug() << "eqf.fileName():" << _extraneousQuotesFile;

      result = true;
    }
  }

  qDebug() << "-- END CMainWindow::processExtraneousQuotesFile()";
  return result;
}


bool CMainWindow::checkExtraneousQuotesOK() {
  bool result;

  if( ui->cbxNmlQuotes->isChecked() ) {
    if( processExtraneousQuotesFile() ) {
      _params.remove( QStringLiteral("inputPath") );
      _params.insert( QStringLiteral("inputPath"), _extraneousQuotesFile );
      _processor->setParams( _params );
      _extraneousQuotesFileExists = true;
      result = true;
    }
    else {
      QMessageBox::critical(
        this,
        QStringLiteral("Input file problem"),
        QString(
          "An unexpected error has been encountered."
          "  Please ensure that your input file is in the correct format, and that you have sufficient disk space and permission to store a copy of the input file."
          "  If problems persist, please contact the developers for further assistance."
        )
      );

      _extraneousQuotesFileExists = false;
      result = false;
    }
  }
  else {
    _extraneousQuotesFileExists = false;
    result = true;
  }

  return result;
}


bool CMainWindow::checkfileHeader() {
  bool result;

  QCsvObject* input = new QCsvObject(
    _params.value( QStringLiteral("inputPath") ).toString(), // filename
    true,                                    // containsFieldList
    true,                                    // stringsContainDelimiters (default value)
    QCsv::LineByLine,                        // readMode (default value)
    true                                     // checkForComment (NOT the default value)
  );

  if( !input->open() ) {
    QMessageBox::critical(
      this,
      QStringLiteral("Cannot open input file"),
      QStringLiteral("The selected input file cannot be opened.  Please ensure that this file is not open in any other application, and check your file permissions.")
    );

    result = false;
  }
  else {
    QStringList fieldsInFile, missingFields;
    QString message;
    // The first call to input->moveNext() reads the
    // header row and establishes the number of fields.

    int nFields = input->moveNext();
    fieldsInFile = input->fieldNames();

    if( ( 0 >= nFields ) || ( 0 == fieldsInFile.count() ) ) {
      QMessageBox::critical(
        this,
        QStringLiteral("Input file appears to be empty"),
        QStringLiteral("The selected input file appears to be empty.  Please ensure that this file is the correct format and is not open in any other application, and check your file permissions.")
      );

      result = false;
    }
    else if( _processor->fieldHeadersOK( fieldsInFile, missingFields ) ) {
      result = true;
    }
    else {
      message = QString(
        "The following fields, required for the BVD database, are not present in the selected input file:"
        "\n\n%1\n\n"
        "Proceed with file processing?"
      ).arg( missingFields.join( QStringLiteral(", ") ) );

      int reply = QMessageBox::question(
        this,
        QStringLiteral("Missing data fields"),
        message
      );

      result = ( QMessageBox::Yes == reply );
    }
  }

  delete input;

  return result;
}


void CMainWindow::run() {
  qDebug() << _params.value( QStringLiteral("outputPath") );
  qDebug() << _params.value( QStringLiteral("errPath") );

  _processor->setParams( _params );

  if( CProcessor::SUCCESS != _processor->result() ) {
    CMessageDialog* dlg = new CMessageDialog(
      QStringLiteral("Could not encrypt file"),
      QStringLiteral("The following problems were encountered:"),
      QMessageBox::Warning,
      this
    );
    dlg->append( _processor->errors() );
    dlg->setWindowModality( Qt::WindowModal );
    dlg->show();
  }
  else {
    // Check paths for input, output, and error files.
    //------------------------------------------------
    if( !QFile::exists( _params.value( QStringLiteral("inputPath") ).toString() ) ) {
      QMessageBox::critical(
        this,
        QStringLiteral("Input file does not exist"),
        QStringLiteral("The indicated input file does not exist.  Please select a different file.")
      );

      return;
    }

    if( QFile::exists( _params.value( QStringLiteral("outputPath") ).toString() ) ) {
      int reply = QMessageBox::question(
        this,
        QStringLiteral("Overwrite output file?"),
        QStringLiteral("An output file with a matching name already exists in the selected folder.\n\nProceed and overwrite existing file?")
      );

      if( !( QMessageBox::Yes == reply ) )
        return;
    }

    if( _params.value( QStringLiteral("logerrors") ).toBool() && QFile::exists( _params.value( QStringLiteral("errPath") ).toString() ) ) {
      int reply = QMessageBox::question(
        this,
        QStringLiteral("Overwrite err log file?"),
        QStringLiteral("An error log file with a matching name already exists in the selected folder.\n\nProceed and overwrite existing file?")
      );

      if( !( QMessageBox::Yes == reply ) )
        return;
    }

    // Check file type.
    //-----------------
    if( !isTextFile() )
      return;

    // Does the file need to have extraneous quotes removed?
    //------------------------------------------------------
    if( !checkExtraneousQuotesOK() )
      return;

    // Check file header for required fields.
    //---------------------------------------
    if( !checkfileHeader() ) {
      if( _extraneousQuotesFileExists )
        QFile::remove( _extraneousQuotesFile );
      return;
    }

    // If we get this far, process the file.
    //--------------------------------------
    this->setControlsEnabled( this, false );

    this->setControlsEnabled( _progress, true );
    _progress->show();

    _processor->readAndEncryptData();


    // Wrap up and go home.
    //---------------------
    //progress.close();
    this->setControlsEnabled( this, true );

    if( _progress->wasCanceled() ) {
      qDebug() << "progress.wasCanceled()";
      if( QFile::exists( _params.value( QStringLiteral("outputPath") ).toString() ) )
        QFile::remove( _params.value( QStringLiteral("outputPath") ).toString() );
      QMessageBox::warning( this, QStringLiteral("Operation canceled"), QStringLiteral("Operation was canceled before completion.") );
    }
    else if( CProcessor::SUCCESS == _processor->result() )
      QMessageBox::information( this, QStringLiteral("Success"), QStringLiteral("File generation is complete.  No validation errors were encountered.") );
    else if( _params.value( QStringLiteral("logerrors") ).toBool() && !(_processor->result() & CProcessor::ERROR_LOG_PROBLEM) )
      QMessageBox::warning( this, QStringLiteral("Success"), QStringLiteral("File generation is complete, but validation errors were encountered.  An error log file has been generated.") );
    else {
      CMessageDialog* dlg = new CMessageDialog(
        QStringLiteral("Problems encountered"),
        QStringLiteral("File generation is complete, but the following problems were encountered:"),
        QMessageBox::Information,
        this
      );
      dlg->append( _processor->errors() );
      dlg->setWindowModality( Qt::WindowModal );
      dlg->show();
    }
  }

  if( _extraneousQuotesFileExists )
    QFile::remove( _extraneousQuotesFile );
}


void CMainWindow::setControlsEnabled( QWidget* widget, const bool val ) {
  widget->setEnabled( val );
  for( int i = 0; i < widget->children().length(); ++i ) {
    if( widget->children().at(i)->isWidgetType() ) {
      setControlsEnabled( (QWidget*)(widget->children().at(i)), val );
    }
  }

  ui->fsErrorFile->setEnabled( ui->cbxWriteErrorLog->isChecked() );
}


void CMainWindow::about() {
  CAboutDialog dlg( this );
  dlg.exec();
}

