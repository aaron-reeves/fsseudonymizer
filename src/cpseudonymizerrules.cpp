/*
Fsseudonymizer - cpseudonymizerrules.h/cpp
------------------------------------------
Begin: 2019/10/12
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2019 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "cpseudonymizerrules.h"

#include <ar_general_purpose/strutils.h>
#include <ar_general_purpose/returncodes.h>
#include <ar_general_purpose/filemagic.h>
#include <ar_general_purpose/log.h>

CPseudonymizerRule::CPseudonymizerRule() {
  initialize();
  _isValid = false;
}


CPseudonymizerRule::CPseudonymizerRule( QCsv* csv ) {
  initialize();

  _rowNumber = csv->currentRowNumber() + 1;

  if( !csv->field( QStringLiteral("FieldName") ).isEmpty() )
    _fieldName = csv->field( QStringLiteral("FieldName") );
  else {
    _isValid = false;
    _errMsgs.append( QStringLiteral("Rules file line %1: 'FieldName' is empty.").arg( _rowNumber ) );
  }

  if( !csv->field( QStringLiteral("StandardPseudonym") ).isEmpty() ) {
    if( strIsBool( csv->field( QStringLiteral("StandardPseudonym") ) ) )
      _pseudonymizeStandard = strToBool( csv->field( QStringLiteral("StandardPseudonym") ) );
    else {
      _isValid = false;
      _errMsgs.append( QStringLiteral("Rules file line %1: 'StandardPseudonym' contains an invalid value.").arg( _rowNumber ) );
    }
  }

  if( !csv->field( QStringLiteral("SimplifiedPseudonym") ).isEmpty() ) {
    if( strIsBool( csv->field( QStringLiteral("SimplifiedPseudonym") ) ) )
      _pseudonymizeSimplified = strToBool( csv->field( QStringLiteral("SimplifiedPseudonym") ) );
    else {
      _isValid = false;
      _errMsgs.append( QStringLiteral("Rules file line %1: 'SimplifiedPseudonym' contains an invalid value.").arg( _rowNumber ) );
    }
  }

  if( _pseudonymizeStandard && _pseudonymizeSimplified ) {
    _isValid = false;
    _errMsgs.append( QStringLiteral("Rules file line %1: 'SimplifiedPseudonym' and 'StandardPseudonym' cannot both be true.").arg( _rowNumber ) );
  }

  if( !csv->field( QStringLiteral("RemoveField") ).isEmpty() ) {
    if( strIsBool( csv->field( QStringLiteral("RemoveField") ) ) )
      _remove = strToBool( csv->field( QStringLiteral("RemoveField") ) );
    else {
      _isValid = false;
      _errMsgs.append( QStringLiteral("Rules file line %1: 'RemoveField' contains an invalid value.").arg( _rowNumber ) );
    }
  }

  if( !csv->field( QStringLiteral("RequiredField") ).isEmpty() ) {
    if( strIsBool( csv->field( QStringLiteral("RequiredField") ) ) )
      _isRequired = strToBool( csv->field( QStringLiteral("RequiredField") ) );
    else {
      _isValid = false;
      _errMsgs.append( QStringLiteral("Rules file line %1: 'RequiredField' contains an invalid value.").arg( _rowNumber ) );
    }
  }

  if( !csv->field( QStringLiteral("ValidateField") ).isEmpty() ) {
    if( strIsBool( csv->field( QStringLiteral("ValidateField") ) ) )
      _validate = strToBool( csv->field( QStringLiteral("ValidateField") ) );
    else {
      _isValid = false;
      _errMsgs.append( QStringLiteral("Rules file line %1: 'ValidateField' contains an invalid value.").arg( _rowNumber ) );
    }
  }

  if( !csv->field( QStringLiteral("ValidationRegExp") ).isEmpty() ) {
    _validationRegExp = QRegExp( csv->field( "ValidationRegExp" ) );
    if( !_validationRegExp.isValid() ) {
      _isValid = false;
      _errMsgs.append( QStringLiteral("Rules file line %1: 'ValidationRegExp' contains an invalid expression.").arg( _rowNumber ) );
    }
  }

  if( _remove && ( _pseudonymizeStandard || _pseudonymizeSimplified || _isRequired || _validate ) ) {
    _isValid = false;
    _errMsgs.append( QStringLiteral("Rules file line %1: A field cannot be both removed and processed.").arg( _rowNumber ) );
  }
}


void CPseudonymizerRule::initialize() {
  _rowNumber = -1;
  _pseudonymizeStandard = false;
  _pseudonymizeSimplified = false;
  _remove = false;
  _isRequired = false;
  _validate = false;

  _isValid = true; // Until shown otherwise
}


void CPseudonymizerRule::assign( const CPseudonymizerRule& other ) {
  _rowNumber = other._rowNumber;

  _fieldName = other._fieldName;
  _pseudonymizeStandard = other._pseudonymizeStandard;
  _pseudonymizeSimplified = other._pseudonymizeSimplified;
  _remove = other._remove;
  _isRequired = other._isRequired;
  _validate = other._validate;
  _validationRegExp = other._validationRegExp;

  _isValid = other._isValid;
}


QString CPseudonymizerRule::sha( const QString& value, const QString& passphrase ) {
  QString tmp = QStringLiteral( "%1 %2" ).arg( passphrase, value );
  return QString( QCryptographicHash::hash( tmp.toLatin1(), QCryptographicHash::Sha224 ).toHex() );
}


// This function MUST NOT change: doing so would break backward compatibility!
QString CPseudonymizerRule::removeAllSymbols( QString str ) const {
  for( int i = str.length() - 1; -1 < i; --i ) {
    if( !( str.at(i).isLetter() || str.at(i).isNumber() || str.at(i).isSpace() ) ) {
      str.replace( i, 1, QString() );
    }
  }

  return str.simplified();
}


QVariant CPseudonymizerRule::process( const QVariant& val, const QString& passphrase, bool& error, QStringList& errMsgs ) const {
  QVariant result = val;

  error = false; // Until shown otherwise

  if( _isRequired && val.isNull() ) {
    error = true;
    errMsgs.append( QStringLiteral("Required value is missing.") );
  }

  if( _validate && !_validationRegExp.exactMatch( val.toString() ) ) {
    error = true;
    errMsgs.append( QStringLiteral("Value does not match the specified regular expression.") );
  }

  if( !error ) {
    if( _pseudonymizeSimplified ) {
      // This function MUST NOT change: doing so would break backward compatibility!
      result = sha( removeAllSymbols( val.toString() ).toLower().trimmed(), passphrase );
    }
    else if( _pseudonymizeStandard ) {
      // This function MUST NOT change: doing so would break backward compatibility!
      result = sha( val.toString().trimmed(), passphrase );
    }
  }

  if( error )
    return QVariant();
  else
    return result;
}


void CPseudonymizerRule::debug() const {
  qDb() << "fieldName:" << _fieldName;
  qDb() << "pseudonymizeStandard:" << _pseudonymizeStandard;
  qDb() << "pseudonymizeSimplified:" << _pseudonymizeSimplified;
  qDb() << "isRequired:" << _isRequired;
  qDb() << "validate:" << _validate;
  qDb() << "validationRegExp:" << _validationRegExp;
  qDb() << "isValidRule:" << _isValid;
}


void CPseudonymizerRules::slotSetStageSteps( const QString& unused, const int nSteps ) {
  Q_UNUSED( unused )
  emit setStageSteps( nSteps );
}


QCsv CPseudonymizerRules::csvFromSpreadsheet( const QString& rulesFileName, const CSpreadsheetWorkBook::SpreadsheetFileFormat format ) {
  QCsv csv;

  CSpreadsheetWorkBook wb( format, rulesFileName );
  connect( &wb, SIGNAL( operationStart( QString, int ) ), this, SLOT( slotSetStageSteps( QString, int ) ) );
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
    csv = wb.sheet( filledSheet ).asCsv( true );
  }
  else if( 0 == nFilledSheets ) {
    logMsg( QStringLiteral("Selected rules spreadsheet contains no data.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
  }
  else {
    logMsg( QStringLiteral("Selected rules spreadsheet contains multiple sheets.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
  }

  disconnect( &wb, SIGNAL( operationStart( QString, int ) ), this, SLOT( slotSetStageSteps( QString, int ) ) );
  disconnect( &wb, SIGNAL( operationProgress( int ) ), this, SIGNAL( setStageStepComplete( int ) ) );

  return csv;
}


CPseudonymizerRules::CPseudonymizerRules(QObject* parent /* = nullptr */) : QObject( parent ), QHash<QString, CPseudonymizerRule>() {
  _result = ReturnCode::UNKNOWN_RESULT;
}


//CPseudonymizerRules::CPseudonymizerRules( const QString& rulesFileName ) : QObject( parent ), QHash<QString, CPseudonymizerRule>() {
//  readFile( rulesFileName );
//}

int CPseudonymizerRules::readFile( const QString &rulesFileName, const bool readFromResource ) {
  _result = ReturnCode::SUCCESS;

  // Does the rules file exist?
  //---------------------------
  if( !QFileInfo::exists( rulesFileName ) ) {
    logMsg( QStringLiteral("Selected rules file does not exist.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
    return _result;
  }

  // Determine the file format and read it
  //--------------------------------------
  QString errMsg;
  bool error;
  bool isCsvFile = false;
  bool isXlsFile = false;
  bool isXlsxFile = false;

  if( !readFromResource ) {
    QString fileTypeInfo = magicFileTypeInfo( rulesFileName, &error, &errMsg );

    if( !error ) {
      isCsvFile = magicStringShowsAnyTextFile( fileTypeInfo );
      isXlsxFile = magicStringShowsXlsxFile( fileTypeInfo, rulesFileName );
      isXlsFile = magicStringShowsXlsFile( fileTypeInfo );

      if( !( isCsvFile || isXlsFile || isXlsxFile ) ) {
        error = true;
        errMsg = QStringLiteral( "File type is not supported." );
      }
    }
  }
  else {
    error = false;

    isCsvFile = rulesFileName.endsWith( QStringLiteral(".csv"), Qt::CaseInsensitive );
    isXlsFile = rulesFileName.endsWith( QStringLiteral(".xls"), Qt::CaseInsensitive );
    isXlsxFile = rulesFileName.endsWith( QStringLiteral(".xlsx"), Qt::CaseInsensitive );

    if( !( isCsvFile || isXlsFile || isXlsxFile ) ) {
      error = true;
      errMsg = QStringLiteral( "File type is not supported." );
    }
  }

  if( error ) {
    logMsg( QStringLiteral("An application error occurred: file type could not be determined. Please check with the developers:") );
    logMsg( errMsg );
    _result = ( _result | ReturnCode::FATAL_ERROR );
    return _result;
  }

  QCsv* csv = nullptr;
  QCsvObject* csvObj = nullptr;
  QCsv csvSpreadsheet;
  bool csvCreated = false;

  if( isCsvFile ) {
    emit setStageSteps( QFileInfo( rulesFileName ).size() );

    csvObj = new QCsvObject( rulesFileName, true, true, QCsv::EntireFile, true );
    connect( csvObj, SIGNAL( nBytesRead( int ) ), this, SIGNAL( setStageStepComplete( int ) ) );

    csvCreated = true;
    if( QCsv::ERROR_NONE != csvObj->error() ) {
      logMsg( QStringLiteral("Rules CSV file could not be processed. Please check the file format:") );
      logMsg( csvObj->errorMsg() );
      _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
    }
    csv = csvObj;
  }
  else if( isXlsxFile ) {
    csvSpreadsheet = csvFromSpreadsheet( rulesFileName, CSpreadsheetWorkBook::Format2007 );
    if( QCsv::ERROR_NONE != csvSpreadsheet.error() ) {
      logMsg( QStringLiteral("Rules Excel file could not be processed. Please check the file format:") );
      logMsg( csvSpreadsheet.errorMsg() );
      _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
    }
    csv = &csvSpreadsheet;
  }
  else if( isXlsFile ) {
    csvSpreadsheet = csvFromSpreadsheet( rulesFileName, CSpreadsheetWorkBook::Format97_2003 );
    if( QCsv::ERROR_NONE != csvSpreadsheet.error() ) {
      logMsg( QStringLiteral("Rules Excel file could not be processed. Please check the file format:") );
      logMsg( csvSpreadsheet.errorMsg() );
      _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
    }
    csv = &csvSpreadsheet;
  }
  else {
    logMsg( QStringLiteral("Rules file type is unrecognized or unsupported.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
  }

  if( ReturnCode::SUCCESS != _result ) {
    if( csvCreated ) {
      disconnect( csvObj, SIGNAL( nBytesRead( int ) ), this, SIGNAL( setStageStepComplete( int ) ) );
      delete csvObj;
    }

    return _result;
  }

  // Check that the rules file has the right columns
  //------------------------------------------------
  QStringList tmpFieldNames;
  foreach( const QString& fieldName, csv->fieldNames() ) {
    tmpFieldNames.append( fieldName.toLower() );
  }

  if( !tmpFieldNames.contains( QStringLiteral("fieldname")) ) {
    logMsg( QStringLiteral("Rules file is missing the required 'FieldName' column.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
  }

  tmpFieldNames.removeOne( QStringLiteral("FieldName").toLower() );
  tmpFieldNames.removeOne( QStringLiteral("StandardPseudonym").toLower() );
  tmpFieldNames.removeOne( QStringLiteral("SimplifiedPseudonym").toLower() );
  tmpFieldNames.removeOne( QStringLiteral("RemoveField").toLower() );
  tmpFieldNames.removeOne( QStringLiteral("RequiredField").toLower() );
  tmpFieldNames.removeOne( QStringLiteral("ValidateField").toLower() );
  tmpFieldNames.removeOne( QStringLiteral("ValidationRegExp").toLower() );

  if( 0 != tmpFieldNames.count() ) {
    logMsg( QStringLiteral("Columns in the rules file are not named correctly.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
  }

  if( ReturnCode::SUCCESS != _result ) {
    if( csvCreated ) {
      disconnect( csvObj, SIGNAL( nBytesRead( int ) ), this, SIGNAL( setStageStepComplete( int ) ) );
      delete csvObj;
    }

    return _result;
  }

  // Generate rules from the rules file
  //-----------------------------------
  csv->toFront();
  while( -1 != csv->moveNext() ) {
    CPseudonymizerRule rule( csv );

    _fieldNames.append( rule.fieldName() );

    if( !rule.isValid() ) {
      _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
      logMsg( rule.errorMessages() );
    }

    this->insert( rule.fieldName().toLower(), rule );
  }

  if( csvCreated ) {
    disconnect( csvObj, SIGNAL( nBytesRead( int ) ), this, SIGNAL( setStageStepComplete( int ) ) );
    delete csvObj;
  }

  return _result;
}


void CPseudonymizerRules::debug() const {
  QHash<QString, CPseudonymizerRule>::const_iterator it;
  for( it = this->begin(); it != this->end(); ++it ) {
    it.value().debug();
    qDb() << endl;
  }
}
