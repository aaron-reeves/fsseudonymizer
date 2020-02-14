/*
Fsseudonymizer - customprocessing.h/cpp - FSS edition
-----------------------------------------------------
Begin: 2019/10/13
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2019 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "customprocessing.h"

#include <ar_general_purpose/log.h>

#include <epic_general_purpose/cph.h>
#include <epic_general_purpose/cpostcode.h>

int customPreProcessData( CTwoDArray<QVariant>* data ) {
  if( data->hasColumnName( QStringLiteral( "CPH" ) ) ) {
    data->appendColumn( QStringLiteral( "County" ) );
  }
  
  if( data->hasColumnName( QStringLiteral( "Producer Postcode" ) ) || data->hasColumnName( QStringLiteral( "ProducerPostcode" ) ) ) {
    data->appendColumn( QStringLiteral( "Postcode district" ) );
  }

  return ReturnCode::SUCCESS;
}


int customPreProcessRow( CTwoDArray<QVariant>* data, const int rowIdx ) {
  int result = ReturnCode::SUCCESS; // Until shown otherwise
  QString errMsg;

  if( data->hasColumnName( QStringLiteral("CPH") ) ) {
    CCPH cph( data->at( QStringLiteral("CPH"), rowIdx ).toString() );
    if( !cph.isValid() ) {
      result = ( result | ReturnCode::DATA_VALIDATION_PROBLEM );
      errMsg = QStringLiteral( "Data does not appear to be a CPH number" );
      // r + 2: add one for indexing, and add another one for the header row
      logMsg( QStringLiteral( "Input file line %1, field '%2': %3" ).arg( rowIdx+2 ).arg( QStringLiteral("CPH"), errMsg ) );
    }
    else {
      data->setValue( QStringLiteral("CPH"), rowIdx, cph.toString() );
      data->setValue( QStringLiteral("County"), rowIdx, cph.county() );
    }
  }

  bool processPostCode = false;
  QString postcodeFieldName;

  if( data->hasColumnName( QStringLiteral("Producer Postcode") ) ) {
    postcodeFieldName = QStringLiteral("Producer Postcode");
    processPostCode = true;
  }

  if( data->hasColumnName( QStringLiteral("ProducerPostcode") ) ) {
    postcodeFieldName = QStringLiteral("ProducerPostcode");
    processPostCode = true;
  }

  if( processPostCode ) {
    CPostcode postcode( data->at( postcodeFieldName, rowIdx ).toString() );
    if( !postcode.isValidFormat() ) {
      result = ( result | ReturnCode::DATA_VALIDATION_PROBLEM );
      errMsg = QStringLiteral( "Data does not appear to be a valid postcode" );
      // r + 2: add one for indexing, and add another one for the header row
      logMsg( QStringLiteral( "Input file line %1, field '%2': %3" ).arg( rowIdx+2 ).arg( QStringLiteral("Producer Postcode"), errMsg ) );
    }
    else {
      data->setValue( QStringLiteral("Producer Postcode"), rowIdx, postcode.postcode() );
      data->setValue( QStringLiteral("Postcode district"), rowIdx, postcode.district() );
    }
  }
  
  return result;
}


int customPostProcessRow( CTwoDArray<QVariant>* pseudonymizedData, const int rowIdx ) {
  Q_UNUSED( pseudonymizedData )
  Q_UNUSED( rowIdx )
  
  return ReturnCode::SUCCESS;
}


int customPostProcessData( CTwoDArray<QVariant>* pseudonymizedData ) {
  Q_UNUSED( pseudonymizedData )
  
  return ReturnCode::SUCCESS;
}
