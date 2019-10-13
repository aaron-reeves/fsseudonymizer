/*
XLPseudonymizer - cprocessor.h/cpp
----------------------------------
Begin: 2019/10/12
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2019 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cprocessor.h"

#include <ar_general_purpose/returncodes.h>
#include <ar_general_purpose/filemagic.h>
#include <ar_general_purpose/debugutils.h>
#include <ar_general_purpose/cfilelist.h>

#include "customprocessing.h"

void CProcessor::slotSetStageSteps( const QString& unused, const qint64 nSteps ) {
  Q_UNUSED( unused )
  emit setStageSteps( nSteps );
}


CProcessor::CProcessor( const QHash<QString, QString>& params, QObject* parent /* = nullptr */ ) : QObject( parent ) {
  _result = ReturnCode::SUCCESS;
  _inputDataFormat = FormatUnspecified;
  _rules = nullptr;

  _params = params;

  if( !QFileInfo::exists( params.value( QStringLiteral("rules") ) ) ) {
    _errMsgs.append( QStringLiteral("Selected rules file does not exist.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
  }

  if( !QFileInfo::exists( params.value( QStringLiteral("input") ) ) ) {
    _errMsgs.append( QStringLiteral("Selected input file does not exist.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
  }
}


CProcessor::~CProcessor() {
  if( nullptr != _rules )
    delete _rules;
}


int CProcessor::readRules() {
  if( !( ReturnCode::SUCCESS == _result ) ) {
    return _result;
  }

  emit stageStarted( QStringLiteral( "Reading rules file..." ) );
  emit setStageStepComplete( 0 );

  _rules = new CPseudonymizerRules();

  _rules->readFile( _params.value( QStringLiteral("rules") ) );
  _errMsgs.append( _rules->errorMessages() );

  emit stageComplete();

  //qDebug() << "Rules:";
  //_rules->debug();

  _result = ( _result | _rules->result() );

  return _result;
}


void CProcessor::getData( const QString& inputFileName ) {
  // Determine the file format and read it
  //--------------------------------------
  QString errMsg;
  bool error;
  QString fileTypeInfo;
  fileTypeInfo = magicFileTypeInfo( inputFileName, &error, &errMsg );

  if( error ) {
    _errMsgs.append( QStringLiteral("An application error occurred: file type could not be determined. Please check with the developers:") );
    _errMsgs.append( errMsg );
    _result = ( _result | ReturnCode::FATAL_ERROR );
    return;
  }

  // If the input file is a CSV file, read it
  //-----------------------------------------
  if( magicStringShowsAsciiTextFile( fileTypeInfo ) ) {
    emit setStageSteps( QFileInfo( inputFileName ).size() );

    QCsvObject csv( inputFileName, true, true, QCsv::EntireFile, true );
    connect( &csv, SIGNAL( nBytesRead( int ) ), this, SIGNAL( setStageStepComplete( int ) ) );

    if( QCsv::ERROR_NONE != csv.error() ) {
      _errMsgs.append( QStringLiteral("Input CSV file could not be processed. Please check the file format:") );
      _errMsgs.append( csv.errorMsg() );
      _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
    }
    else {
      _inputDataFormat = FormatCSV;

      _data.setSize( csv.fieldCount(), 0 );
      _data.setColNames( csv.fieldNames() );
      csv.toFront();
      while( -1 != csv.moveNext() ) {
        QVariantList row;
        for( int c = 0; c < csv.fieldCount(); ++c ) {
          row.append( csv.field( c ) );
        }
        _data.appendRow( row );
      }
    }

    disconnect( &csv, SIGNAL( nBytesRead( int ) ), this, SIGNAL( setStageStepComplete( int ) ) );
  }

  // If the file is an Excel spreadsheet, read it
  //---------------------------------------------
  else if( magicStringShowsXlsxFile( fileTypeInfo, inputFileName ) || magicStringShowsXlsFile( fileTypeInfo ) ) {
    CSpreadsheetWorkBook wb( inputFileName );
    connect( &wb, SIGNAL( operationStart( QString, int ) ), this, SLOT( slotSetStageSteps( QString, qint64 ) ) );
    connect( &wb, SIGNAL( operationProgress( int ) ), this, SIGNAL( setStageStepComplete( int ) ) );

    // The spreadsheet file should have exactly 1 sheet that contains data.
    // If this is not the case, we don't currently know what to do.
    if( wb.isOpen() ) {
      wb.readAllSheets();
    }

    int nFilledSheets = 0;
    int filledSheet = 0;
    for( int i = 0; i < wb.sheetCount(); ++i ) {
      if( !wb.sheet( i ).isEmpty() ) {
        filledSheet = i;
        ++nFilledSheets;
      }
    }

    if( 1 == nFilledSheets ) {
      _inputDataFormat = FormatExcel;
      _data = wb.sheet( filledSheet ).data( true );
    }
    else if( 0 == nFilledSheets ) {
      _errMsgs.append( QStringLiteral("Selected input spreadsheet contains no data.") );
      _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
    }
    else {
      _errMsgs.append( QStringLiteral("Selected input spreadsheet contains multiple sheets.") );
      _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
    }

    disconnect( &wb, SIGNAL( operationStart( QString, int ) ), this, SLOT( slotSetStageSteps( QString, qint64 ) ) );
    disconnect( &wb, SIGNAL( operationProgress( int ) ), this, SIGNAL( setStageStepComplete( int ) ) );
  }

  // If the file is something else, it's a problem
  //----------------------------------------------
  else {
    _errMsgs.append( QStringLiteral("Input file type is unrecognized or unsupported.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
  }
}


int CProcessor::readData() {
  if( !( ReturnCode::SUCCESS == _result ) ) {
    return _result;
  }

  emit stageStarted( QStringLiteral( "Reading data file..." ) );
  emit setStageStepComplete( 0 );

  getData( _params.value( QStringLiteral("input") ) );

  if( ReturnCode::SUCCESS == _result ) {
    foreach( const QString& fieldName, _rules->fieldNames() ) {
      if( !_data.colNames().contains( fieldName, Qt::CaseInsensitive ) ) {
        _errMsgs.append( QStringLiteral( "Data file does not contain field '%1'").arg( fieldName ) );
        _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
      }
    }
  }

  emit stageComplete();

  return _result;
}


int CProcessor::process() {
  _pseudonymizedData.clear();

  if( ReturnCode::SUCCESS != _result ) {
    return _result;
  }

  //------------------------------------------------------
  // If needed, any further preprocessing of the data set
  // can be done in a customized version of this function
  //------------------------------------------------------
  _result = ( _result | customPreProcessData( &_data ) );
  //------------------------------------------------------

  _pseudonymizedData.setSize( _data.nCols(), _data.nRows() );
  _pseudonymizedData.setColNames( _data.colNames() );

  emit stageStarted( QStringLiteral( "Processing data..." ) );
  emit setStageStepComplete( 0 );
  emit setStageSteps( _data.nRows() );

  for( int r = 0; r < _data.nRows(); ++r ) {

    //---------------------------------------------------------
    // If needed, any further preprocessing of a particular row
    // can be done in a customized version of this function
    //---------------------------------------------------------
    _result = ( _result | customPreProcessRow( &_data, r ) );
    //---------------------------------------------------------

    for( int c = 0; c < _data.nCols(); ++c ) {
      if( _rules->fieldNames().contains( _data.colNames().at(c), Qt::CaseInsensitive ) ) {
        bool error;
        QStringList errMsgs;
        QVariant val = _rules->value( _data.colNames().at(c).toLower() ).process( _data.at( c, r ), _params.value( QStringLiteral("passphrase") ), error, errMsgs );

        //qDebug() << error << errMsg << val;

        if( error ) {
          foreach( const QString& errMsg, errMsgs ) {
            _errMsgs.append( QStringLiteral( "Input file line %1, field '%2': %3" ).arg( r+1 ).arg( _data.colNames().at(c), errMsg ) );
          }
          _result = ( _result | ReturnCode::DATA_VALIDATION_PROBLEM );
        }
        _pseudonymizedData.setValue( c, r, val );
      }
      else {
        //qDebug() << "Column name not found:" << _data.colNames().at(c);
        _pseudonymizedData.setValue( c, r, _data.at( c, r ) );
      }
    }

    //---------------------------------------------------------------------
    // If needed, any further post-processing of a particular row
    // can be done in a customized version of this function
    //---------------------------------------------------------------------
    _result = ( _result | customPostProcessRow( &_pseudonymizedData, r ) );
    //---------------------------------------------------------------------

    emit setStageStepComplete( r + 1 );
  }

  //-------------------------------------------------------------------
  // If needed, any further post-processing of the data set
  // can be done in a customized version of this function
  //-------------------------------------------------------------------
  _result = ( _result | customPostProcessData( &_pseudonymizedData ) );
  //-------------------------------------------------------------------

  debugArray( _pseudonymizedData );

  emit stageComplete();

  return _result;
}


int CProcessor::writeOutput() {
  if( ReturnCode::SUCCESS != _result ) {
    return _result;
  }

  QString outputFileName;
  if(  _params.value( QStringLiteral("output") ).isEmpty() ) {
    CPathString filePath( _params.value( QStringLiteral("input") ) );
    outputFileName = QStringLiteral("%1%2-%3.%4").arg( filePath.directory(), filePath.baseName(), QStringLiteral("pseudonymized"), filePath.fileType() );
  }
  else {
    outputFileName = _params.value( QStringLiteral("output") );
  }

  // Can we write the file?
  //-----------------------
  QFileInfo fi( outputFileName );

  if( fi.exists() && !QFileInfo( outputFileName ).isWritable() ) {
    _errMsgs.append( QStringLiteral( "Cannot write to the selected output file (err 1). Please check your permissions." ) );
    _result = ( _result | ReturnCode::OUTPUT_FILE_PROBLEM );
    return _result;
  }
  else if( !QFileInfo( fi.dir().absolutePath() ).isWritable() ) {
    _errMsgs.append( QStringLiteral( "Cannot write to the selected output file (err 2). Please check your permissions." ) );
    _result = ( _result | ReturnCode::OUTPUT_FILE_PROBLEM );
    return _result;
  }

  // Which file format should be written?
  //-------------------------------------
  if(
    outputFileName.endsWith( QStringLiteral(".csv"), Qt::CaseInsensitive )
    || ( FormatCSV == _inputDataFormat )
  ) {
    CPathString filePath( outputFileName );
    outputFileName = QStringLiteral("%1%2.%3").arg( filePath.directory(), filePath.baseName(), QStringLiteral("csv") );
    writeOutputCsv( outputFileName );
  }
  else if(
    outputFileName.endsWith( QStringLiteral(".xlsx"), Qt::CaseInsensitive )
    || ( FormatExcel == _inputDataFormat )
  ) {
    CPathString filePath( outputFileName );
    outputFileName = QStringLiteral("%1%2.%3").arg( filePath.directory(), filePath.baseName(), QStringLiteral("xlsx") );
    writeOutputXlsx( outputFileName );
  }

  return _result;
}


QStringList CProcessor::fileHeader() {
  QStringList result;

  result.append( QStringLiteral( "# %1 pseudonymized data file" ).arg( qApp->applicationName() ) );
  result.append( QStringLiteral( "# Application version: %1" ).arg( qApp->applicationVersion() ) );
  result.append( QStringLiteral( "# Generation date: %1" ).arg( QDateTime::currentDateTime().toString( QStringLiteral("dd MMM yyyy hh:mm:ss") ) ) );
  result.append( QStringLiteral( "# Generated by: %1 <%2>").arg( _params.value( QStringLiteral("username") ), _params.value( QStringLiteral("email") ) ) );
  result.append( QStringLiteral( "# Encryption validity check: %1" ).arg( CPseudonymizerRule::sha( QStringLiteral("This will catch errors if multiple files are encrypted with the same phrase"), _params.value( QStringLiteral("passphrase") ) ) ) );

  return result;
}


void CProcessor::writeOutputXlsx( const QString& outputFileName ) {
  emit stageStarted( QStringLiteral("Writing output file...") );
  emit setStageStepComplete( 0 );

  CSpreadsheet sheet( _pseudonymizedData );

  QStringList header = this->fileHeader();

  for( int i = 0; i < header.count() + 1; ++i ) {
    sheet.prependRow();
  }

  for( int r = 0; r < header.count(); ++r ) {
    sheet.setValue( 0, r, CSpreadsheetCell( header.at(r) ) );
  }

  if( !sheet.writeXlsx( outputFileName, true ) ) {
    _errMsgs.append( QStringLiteral( "Cannot write to the selected output file (err 3). Please check your permissions." ) );
    _result = ( _result | ReturnCode::OUTPUT_FILE_PROBLEM );
  }

  emit stageComplete();
}


void CProcessor::writeOutputCsv( const QString& outputFileName ) {
  QFile file( outputFileName );
  if( !file.open( QIODevice::WriteOnly ) ) {
    _errMsgs.append( QStringLiteral( "Cannot write to the selected output file (err 4). Please check your permissions." ) );
    _result = ( _result | ReturnCode::OUTPUT_FILE_PROBLEM );
    return;
  }

  emit stageStarted( QStringLiteral("Writing output file...") );
  emit setStageStepComplete( 0 );
  emit setStageSteps( _pseudonymizedData.nRows() );

  QTextStream out(&file);
  //if( !codec.isEmpty() )
  //  out.setCodec(codec.toLatin1());

  foreach( const QString& line, this->fileHeader() ) {
    out << line << "\r\n";
  }
  out << "\r\n";

  QStringList output;

  // Write the field names row first...
  output = CSV::csvStringList( _pseudonymizedData.colNames(),  QChar( ',' ), CSV::OriginalCase );
  out << output.join( QChar( ',' ) ) << "\r\n";

  // Then write the data.
  for( int r = 0; r < _pseudonymizedData.nRows(); ++r ) {
    output.clear();
    for( int c = 0; c < _pseudonymizedData.nCols(); ++c ) {
      output.append( _pseudonymizedData.value( c, r ).toString() );
    }
    output = CSV::csvStringList( output,  QChar( ',' ), CSV::OriginalCase );
    out << output.join(  QChar( ',' ) ) << "\r\n";

    emit setStageStepComplete( r + 1 );
  }

  file.close();

  emit stageComplete();
}


