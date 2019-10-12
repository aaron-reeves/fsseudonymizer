/*
XLPseudonymizer - cpseudonymizerrules.h/cpp
-------------------------------------------
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

CPseudonymizerRule::CPseudonymizerRule() {
  initialize();
  _isValid = false;
}


CPseudonymizerRule::CPseudonymizerRule( QCsv* csv ) {
  _rowNumber = csv->currentRowNumber() + 1;

  if( !csv->field( QStringLiteral("FieldName") ).isEmpty() )
    _fieldName = csv->field( QStringLiteral("FieldName") );
  else {
    _isValid = false;
    _errMsgs.append( QStringLiteral("'FieldName' is empty.") );
  }

  if( !csv->field( QStringLiteral("GeneratePseudonym") ).isEmpty() ) {
    if( strIsBool( csv->field( QStringLiteral("GeneratePseudonym") ) ) )
      _pseudonymize = strToBool( csv->field( QStringLiteral("GeneratePseudonym") ) );
    else {
      _isValid = false;
      _errMsgs.append( QStringLiteral("'GeneratePseudonym' contains an invalid value.") );
    }
  }

  if( !csv->field( QStringLiteral("RequiredField") ).isEmpty() ) {
    if( strIsBool( csv->field( QStringLiteral("RequiredField") ) ) )
      _pseudonymize = strToBool( csv->field( QStringLiteral("RequiredField") ) );
    else {
      _isValid = false;
      _errMsgs.append( QStringLiteral("'RequiredField' contains an invalid value.") );
    }
  }

  if( !csv->field( QStringLiteral("ValidateField") ).isEmpty() ) {
    if( strIsBool( csv->field( QStringLiteral("ValidateField") ) ) )
      _pseudonymize = strToBool( csv->field( QStringLiteral("ValidateField") ) );
    else {
      _isValid = false;
      _errMsgs.append( QStringLiteral("'ValidateField' contains an invalid value.") );
    }
  }

  if( !csv->field( QStringLiteral("ValidationRegExp") ).isEmpty() ) {
    _validationRegExp = QRegExp( csv->field( "ValidationRegExp" ) );
    if( !_validationRegExp.isValid() ) {
      _isValid = false;
      _errMsgs.append( QStringLiteral("'ValidationRegExp' contains an invalid expression.") );
    }
  }
}


void CPseudonymizerRule::initialize() {
  _pseudonymize = false;
  _isRequired = false;
  _validate = false;

  _isValid = true; // Until shown otherwise
}


void CPseudonymizerRule::assign( const CPseudonymizerRule& other ) {
  _fieldName = other._fieldName;
  _pseudonymize = other._pseudonymize;
  _isRequired = other._isRequired;
  _validate = other._validate;
  _validationRegExp = other._validationRegExp;
}


QString CPseudonymizerRule::sha( const QString& value, const QString& passphrase ) const {
  QString tmp = QStringLiteral( "%1 %2" ).arg( passphrase, value );
  return QString( QCryptographicHash::hash( tmp.toLatin1(), QCryptographicHash::Sha224 ).toHex() );
}


QVariant CPseudonymizerRule::process( const QVariant& val, const QString& passphrase, bool& error, QString& errMsg ) const {
  QVariant result = val;

  error = false; // Until shown otherwise
  QStringList errs;

  if( _isRequired && val.isNull() ) {
    error = true;
    errs.append( QStringLiteral("Required value is missing.") );
  }

  if( _validate && !_validationRegExp.exactMatch( val.toString() ) ) {
    error = true;
    errs.append( QStringLiteral("Value does not match the specified regular expression.") );
  }

  if( !error && _pseudonymize ) {
    result = sha( val.toString().simplified().trimmed().toLower(), passphrase );
  }

  if( error ) {
    errMsg = errs.join( QStringLiteral( " " ) );
    return QVariant();
  }
  else {
    return result;
  }
}


void CPseudonymizerRule::debug() const {
  qDebug() << "fieldName:" << _fieldName;
  qDebug() << "pseudonymize:" << _pseudonymize;
  qDebug() << "isRequired:" << _isRequired;
  qDebug() << "validate:" << _validate;
  qDebug() << "validationRegExp:" << _validationRegExp;
}


QCsv CPseudonymizerRules::csvFromSpreadsheet( const QString& rulesFileName ) {
  QCsv csv;

  CSpreadsheetWorkBook wb( rulesFileName );

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
    _errMsgs.append( QStringLiteral("Selected rules spreadsheet contains no data.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
  }
  else {
    _errMsgs.append( QStringLiteral("Selected rules spreadsheet contains multiple sheets.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
  }

  return csv;
}


CPseudonymizerRules::CPseudonymizerRules() : QHash<QString, CPseudonymizerRule>() {
  _result = ReturnCode::UNKNOWN_RESULT;
}


CPseudonymizerRules::CPseudonymizerRules( const QString& rulesFileName ) : QHash<QString, CPseudonymizerRule>() {
  _result = ReturnCode::SUCCESS;

  if( !QFileInfo::exists( rulesFileName ) ) {
    _errMsgs.append( QStringLiteral("Selected rules file does not exist.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
    return;
  }

  QString errMsgTxt, errMsgXls, errMsgXlsx;

  bool isTextFile = magicIsAsciiTextFile( rulesFileName, nullptr, nullptr, &errMsgTxt );
  bool isXlsFile = magicIsXlsFile( rulesFileName, nullptr, nullptr, &errMsgXls );
  bool isXlsxFile = magicIsXlsxFile( rulesFileName, nullptr, nullptr, &errMsgXlsx );

  QCsv csv;

  if( isTextFile ) {
    csv = QCsv( rulesFileName, true, true, QCsv::EntireFile, true );
    if( QCsv::ERROR_NONE != csv.error() ) {
      _errMsgs.append( QStringLiteral("Rules CSV file could not be processed. Please check the file format:") );
      _errMsgs.append( csv.errorMsg() );
      _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
    }
  }
  else if( isXlsFile || isXlsxFile ) {
    csv = csvFromSpreadsheet( rulesFileName );
    if( QCsv::ERROR_NONE != csv.error() ) {
      _errMsgs.append( QStringLiteral("Rules Excel file could not be processed. Please check the file format:") );
      _errMsgs.append( csv.errorMsg() );
      _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
    }
  }
  else {
    _errMsgs.append( QStringLiteral("Rules file type is unrecognized or unsupported.") );
    _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
  }

  if( ReturnCode::SUCCESS != _result ) {
    return;
  }

  csv.toFront();
  while( -1 != csv.moveNext() ) {
    CPseudonymizerRule rule( &csv );

    _fieldNames.append( rule.fieldName() );

    if( !rule.isValid() ) {
      _result = ( _result | ReturnCode::INPUT_FILE_PROBLEM );
      _errMsgs.append( rule.errorMessage() );
    }

    this->insert( rule.fieldName().toLower(), rule );
  }
}


CPseudonymizerRules::CPseudonymizerRules( const CPseudonymizerRules& other ) : QHash<QString, CPseudonymizerRule>( other ) {
  _result = other._result;
  _fieldNames = other._fieldNames;
  _errMsgs = other._errMsgs;
}


CPseudonymizerRules& CPseudonymizerRules::operator=( const CPseudonymizerRules& other ) {
  QHash<QString, CPseudonymizerRule>::operator=( other );

  _result = other._result;
  _fieldNames = other._fieldNames;
  _errMsgs = other._errMsgs;

  return *this;
}
