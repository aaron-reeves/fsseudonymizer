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
    QCsv csv( inputFileName, true, true, QCsv::EntireFile, true );
    if( QCsv::ERROR_NONE != csv.error() ) {
      _errMsgs.append( QStringLiteral("Input CSV file could not be processed. Please check the file format:") );
      _errMsgs.append( csv.errorMsg() );
      _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
    }
    else {
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
  }

  // If the file is an Excel spreadsheet, read it
  //---------------------------------------------
  else if( magicStringShowsXlsxFile( fileTypeInfo, inputFileName ) || magicStringShowsXlsFile( fileTypeInfo ) ) {
    CSpreadsheetWorkBook wb( inputFileName );

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
  }

  // If the file is something else, it's a problem
  //----------------------------------------------
  else {
    _errMsgs.append( QStringLiteral("Input file type is unrecognized or unsupported.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
  }
}


CProcessor::CProcessor(const QHash<QString, QString>& params ) {
  _result = ReturnCode::SUCCESS;

  _params = params;

  if( !QFileInfo::exists( params.value( QStringLiteral("rules") ) ) ) {
    _errMsgs.append( QStringLiteral("Selected rules file does not exist.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
  }

  if( !QFileInfo::exists( params.value( QStringLiteral("input") ) ) ) {
    _errMsgs.append( QStringLiteral("Selected input file does not exist.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
  }

  _rules = CPseudonymizerRules( params.value( QStringLiteral("rules") ) );
  _errMsgs.append( _rules.errorMessages() );

  getData( params.value( QStringLiteral("input") ) );

  //qDebug() << "Rules:";
  //_rules.debug();

  //qDebug() << "Data:";
  //debugArray( _data );

  _result = ( _result | _rules.result() );

  if( ReturnCode::SUCCESS == _result ) {
    foreach( const QString& fieldName, _rules.fieldNames() ) {
      if( !_data.colNames().contains( fieldName, Qt::CaseInsensitive ) ) {
        _errMsgs.append( QStringLiteral( "Data file does not contain field '%1'").arg( fieldName ) );
        _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
      }
    }
  }

  //qDebug() << "Result after construction:" << _result;
}


int CProcessor::run() {
  _pseudonymizedData.clear();

  if( ReturnCode::SUCCESS != _result ) {
    return _result;
  }

  _pseudonymizedData.setSize( _data.nCols(), _data.nRows() );
  _pseudonymizedData.setColNames( _data.colNames() );

  for( int r = 0; r < _data.nRows(); ++r ) {
    for( int c = 0; c < _data.nCols(); ++c ) {
      if( _rules.fieldNames().contains( _data.colNames().at(c), Qt::CaseInsensitive ) ) {
        bool error;
        QStringList errMsgs;
        QVariant val = _rules.value( _data.colNames().at(c).toLower() ).process( _data.at( c, r ), _params.value( QStringLiteral("passphrase") ), error, errMsgs );

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
  }

  //debugArray( _pseudonymizedData );

  return _result;
}


