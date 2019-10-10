#include "cprocessor.h"

#include <BVDShared/cbvddata.h>
#include <BVDShared/cbvdrecord.h>

#include <ar_general_purpose/strutils.h>
#include <ar_general_purpose/log.h>
#include <ar_general_purpose/filemagic.h>

CProcessor::CProcessor( const bool enforceRequiredFields, QObject* parent ) : QObject( parent ) {
  initialize( enforceRequiredFields );
}


CProcessor::CProcessor( const QHash<QString, QVariant>& params, const bool enforceRequiredFields, QObject* parent ) : QObject( parent ) {
  initialize( enforceRequiredFields );
  setParams( params );
}


CProcessor::~CProcessor() {
  clearGlobalFieldList();
}


void CProcessor::initialize( const bool enforceRequiredFields ) {
  resetResult();
  _params.clear();

  _enforceRequiredFields = enforceRequiredFields;

  // Set up the list of fields, which will be used by everything else.
  //------------------------------------------------------------------
  buildGlobalFieldList();
}


void CProcessor::setParams( const QHash<QString, QVariant>& params ){
  resetResult();
  _params = params;

  if( _params.contains( QStringLiteral("nml") ) && _params.value( QStringLiteral("nml") ).toBool() )
    globalFieldList.at( QStringLiteral("lab_test_reference") )->setRequiresEncryption( true );
  else
    globalFieldList.at( QStringLiteral("lab_test_reference") )->setRequiresEncryption( false );

  validateParams();
}


void CProcessor::resetResult() {
  _result = SUCCESS;
  _errMsgs.clear();
}


bool CProcessor::silent() {
  return _params.value( QStringLiteral("silent") ).toBool();
}


QFileInfo CProcessor::validateSharedParams() {
  QFileInfo inputFileInfo;

  if( !_params.contains( QStringLiteral("inputPath") ) || _params.value( QStringLiteral("inputPath") ).isNull() ) {
    handleError( CError::Fatal, QStringLiteral("Input file is not specified.") );
    _result = ( _result | CProcessor::INPUT_FILE_PROBLEM );
  }
  else {
    inputFileInfo = QFileInfo( _params.value( QStringLiteral("inputPath") ).toString() );

    if( !inputFileInfo.exists() ) {
      handleError( CError::Fatal, QStringLiteral("Specified input file does not exist.") );
      _result = ( _result | CProcessor::INPUT_FILE_PROBLEM );
    }
  }

  if( !_params.contains( QStringLiteral("errPath") ) && _params.contains( QStringLiteral("inputPath") ) )
    _params.insert( QStringLiteral("errPath"), QStringLiteral( "%1/%2.err" ).arg( inputFileInfo.dir().path(), inputFileInfo.baseName() ) );

  return inputFileInfo;
}


void CProcessor::validateParams() {
  QFileInfo inputFileInfo = validateSharedParams();

  if( !_params.contains( QStringLiteral("passphrase") ) || _params.value( QStringLiteral("passphrase") ).isNull() ) {
    handleError( CError::Fatal, QStringLiteral("Pass phrase is not specified.") );
    _result = ( _result | CProcessor::BAD_COMMAND );
  }
  else if( 5 > _params.value( QStringLiteral("passphrase") ).toString().length() ) {
    handleError( CError::Fatal, QStringLiteral("Pass phrase is too short.") );
    _result = ( _result | CProcessor::BAD_COMMAND );
  }

  if( !_params.contains( QStringLiteral("userName") ) || _params.value( QStringLiteral("userName") ).isNull() ) {
    handleError( CError::Fatal, QStringLiteral("Name of user is not specified.") );
    _result = ( _result | CProcessor::BAD_COMMAND );
  }
  else if( 3 > _params.value( QStringLiteral("userName") ).toString().length() ) {
    handleError( CError::Fatal, QStringLiteral("Name of user is too short.") );
    _result = ( _result | CProcessor::BAD_COMMAND );
  }

  if( !_params.contains( QStringLiteral("userEmail") ) || _params.value( QStringLiteral("userEmail") ).isNull() ) {
    handleError( CError::Fatal, QStringLiteral("User email address is not specified.") );
    _result = ( _result | CProcessor::BAD_COMMAND );
  }
  else if( !isEmailAddress( _params.value( QStringLiteral("userEmail") ).toString() ) ) {
    handleError( CError::Fatal, QStringLiteral("User email address is not valid.") );
    _result = ( _result | CProcessor::BAD_COMMAND );
  }

  if( !_params.contains( QStringLiteral("outputPath") ) && _params.contains( QStringLiteral("inputPath") ) )
    _params.insert( QStringLiteral("outputPath"), QStringLiteral( "%1/%2.bvd" ).arg( inputFileInfo.dir().path(), inputFileInfo.baseName() ) );
}


void CProcessor::interrupt() {
  _processingInterrupted = true;
  _result = ( _result | PROCESSING_INTERRUPTED );
  handleError( CError::Warning, QStringLiteral("File processing was interrupted by the user.") );
  emit processInterrupted();
}


void CProcessor::bytesProcessed( const int val ) {
  _totalBytesProcessed = _totalBytesProcessed + val;
  qApp->processEvents();
  emit setProgressValue( _totalBytesProcessed );
}


void CProcessor::linesWritten( const int val ) {
  qApp->processEvents();
  emit setProgressValue( val );
}


bool CProcessor::fieldHeadersOK( const QStringList& fieldsInFile, QStringList& missingFields ) {
  return( globalFieldList.fieldHeadersOK( fieldsInFile, missingFields ) );
}


void CProcessor::readInputFile( CBVDData* bvdData ) {
  _totalBytesProcessed = 0;
  _processingInterrupted = false;

  QFileInfo fi( _params.value( QStringLiteral("inputPath")).toString() );

  if( 0 == fi.size() ) {
    handleError( CError::Critical, QStringLiteral( "The selected input file appears to be empty: %1" ).arg( _params.value( QStringLiteral("inputPath") ).toString() ) );
    _result = ( _result | CProcessor::INPUT_FILE_PROBLEM );
  }
  else if( !magicIsAsciiTextFile( _params.value( QStringLiteral("inputPath") ).toString() ) ) {
    handleError( CError::Critical, QStringLiteral( "The selected input file does not appear to be plain-text CSV: %1" ).arg( _params.value( QStringLiteral("inputPath") ).toString() ) );
    _result = ( _result | CProcessor::INPUT_FILE_PROBLEM );
  }
  else {
    // Attempt to open CSV input file
    //-------------------------------
    QCsvObject* input = new QCsvObject(
      _params.value( QStringLiteral("inputPath") ).toString(), // filename
      true,                                    // containsFieldList
      true,                                    // stringsContainDelimiters (default value)
      QCsv::LineByLine,                        // readMode (default value)
      true                                     // checkForComment (NOT the default value)
    );

    if( !input->open() ) {
      handleError( CError::Fatal, QStringLiteral( "Cannot open input file for reading: %1" ).arg( _params.value( QStringLiteral("inputPath") ).toString() ) );
      _result = ( _result | INPUT_FILE_PROBLEM );
    }
    else {
      // The first call to input->moveNext() reads the
      // header row and establishes the number of fields.
      int nFields = input->moveNext();

      if( ( 0 >= nFields ) || ( 0 == input->fieldNames().count() ) ) {
        handleError( CError::Critical, QStringLiteral( "The selected input file appears to be empty: %1" ).arg( _params.value( QStringLiteral("inputPath") ).toString() ) );
        _result = ( _result | CProcessor::INPUT_FILE_PROBLEM );
      }
      else {
        // See if required fields are all present.
        //----------------------------------------
        QStringList missingFields;
        if( _enforceRequiredFields && !fieldHeadersOK( input->fieldNames(), missingFields ) )  {
          handleError( CError::Fatal, QStringLiteral( "The following required fields are missing: %1" ).arg( missingFields.join( QStringLiteral(", ") ) ) );
          _result = ( _result | CProcessor::REQUIRED_FIELDS_MISSING );
        }
        else {
          // Process each line of the input file, performing data validation along the way.
          //-------------------------------------------------------------------------------
          connect( this, SIGNAL( processInterrupted() ), bvdData, SLOT( interrupt() ) );
          connect( bvdData, SIGNAL( nLinesWritten( int ) ), this, SLOT( linesWritten( int ) ) );
          connect( bvdData, SIGNAL( nBytesRead( int ) ), this, SLOT( bytesProcessed( int ) ) );

          emit setProgressLabelText( QStringLiteral("Processing input file...") );
          emit setProgressMax( int( fi.size() ) );

          bool excludeParish = false;
          if( _params.contains( QStringLiteral("pchs") ) && _params.value( QStringLiteral("pchs") ).toBool() ) {
            excludeParish = true;
          }

          bvdData->addRecords( input, _params.value( QStringLiteral("passphrase") ).toString(), excludeParish );

          handleError( bvdData->errMsgs() );
          _result = ( _result | bvdData->result() );
        }
      }
    }

    delete input;
  }
}


void CProcessor::writeEncryptedOutputFile( CBVDData* bvdData ) {
  emit setProgressLabelText( QStringLiteral("Writing output file...") );
  emit setProgressValue( 0 );
  emit setProgressMax( bvdData->count() );

  bool excludeParish = false;
  if( _params.contains( QStringLiteral("pchs") ) && _params.value( QStringLiteral("pchs") ).toBool() ) {
    excludeParish = true;
  }

  if(
    !bvdData->writeOutput(
      _params.value( QStringLiteral("outputPath") ).toString(),
      _params.value( QStringLiteral("userName") ).toString(),
      _params.value( QStringLiteral("userEmail") ).toString(),
      excludeParish
    )
  ) {
    handleError( CError::Fatal, QStringLiteral( "Cannot open output file for writing: %1" ).arg( _params.value( QStringLiteral("outputPath") ).toString() ) );
    _result = ( _result | OUTPUT_FILE_PROBLEM );
  }
  else
    _result = ( _result | bvdData->result() );
}


void CProcessor::writeErrorLogFile() {
  emit setProgressLabelText( QStringLiteral("Writing validation log...") );
  emit setProgressMax( _errMsgs.count() );
  emit setProgressValue( 0 );

  QFile errorFile( _params.value( QStringLiteral("errPath") ).toString() );
  if( !errorFile.open( QFile::WriteOnly | QFile::Truncate ) ) {
    qDebug() << QStringLiteral( "Cannot open error log for writing: %1" ).arg( _params.value( QStringLiteral("errPath") ).toString() );
    handleError( CError::Critical, QStringLiteral( "Cannot open error log for writing: %1" ).arg( _params.value( QStringLiteral("errPath") ).toString() ) );
    _result = ( _result | ERROR_LOG_PROBLEM );
  }
  else {
    QTextStream out( &errorFile );

    if( 0 == _errMsgs.count() ) {
      out << "(No errors encountered)\r\n";
    }
    else {
      for( int i = 0; i < _errMsgs.count(); ++i ) {
        out << _errMsgs.at(i) << "\r\n";
        emit setProgressValue( i+1 );
      }
    }

    out << flush;
    errorFile.close();
  }
}


int CProcessor::readAndEncryptData() {
  resetResult();

  CBVDData* bvdData = new CBVDData();

  readInputFile( bvdData );

  // If the file was read successfully, write the encrypted output.
  if( !( _result & INPUT_FILE_PROBLEM ) && !( _result & REQUIRED_FIELDS_MISSING ) && ( nullptr != bvdData) && !bvdData->processInterrupted() )
    writeEncryptedOutputFile( bvdData );

  // Write a validation error log file, if requested.
  if( !( _result & INPUT_FILE_PROBLEM ) && !bvdData->processInterrupted() && _params.value( QStringLiteral("logerrors") ).toBool() )
    writeErrorLogFile();

  delete bvdData;

  return _result;
}


void CProcessor::handleError( CError::ErrorType type, const QString& msg ) {
  _errMsgs.append( CError( type, msg ) );
  logMsg( msg );

  if( CError::Fatal == type ) {
    _result = ( _result | CProcessor::FATAL_ERROR );
  }
}


void CProcessor::handleError( const CErrorList& msgs ) {
  for( int i = 0; i < msgs.count(); ++i )
    handleError( msgs.itemAt(i).type(), msgs.itemAt(i).msg() );
}


void CProcessor::debugParams() {
  QHashIterator<QString, QVariant> it( _params );

  qDebug();
  qDebug() << "--- PARAMS:";
  while( it.hasNext() ) {
    it.next();
    qDebug() << "  " << it.key() << it.value().toString();
  }

}



