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


#ifndef CPSEUDONYMIZERRULES_H
#define CPSEUDONYMIZERRULES_H

#include <QtCore>

#include <ar_general_purpose/arxl.h>

class CPseudonymizerRule {
  public:
    CPseudonymizerRule();
    CPseudonymizerRule( QCsv* csv );
    CPseudonymizerRule( const CPseudonymizerRule& other ) { assign( other ); }
    CPseudonymizerRule& operator=( const CPseudonymizerRule& other ) { assign( other ); return *this; }
    ~CPseudonymizerRule() { /* Nothing to do here */ }

    QString fieldName( void ) const { return _fieldName; }
    bool pseudonymize( void ) const { return _pseudonymize; }
    bool isRequired( void ) const { return _isRequired; }
    bool validate( void ) const { return _validate; }
    QRegExp validationRegExp( void ) const { return _validationRegExp; }

    bool isValid() const { return _isValid; }
    QString errorMessage() const { return QStringLiteral( "Line %1: %2" ).arg( _rowNumber ).arg( _errMsgs.join( QStringLiteral( " " ) ) ); }

    QVariant process( const QVariant& val, const QString& passphrase, bool& error, QString& errMsg ) const;

    void debug() const;

  protected:
    void initialize();
    void assign( const CPseudonymizerRule& other );

    QString sha( const QString& value, const QString& passphrase ) const;

    int _rowNumber;

    QString _fieldName;
    bool _pseudonymize;
    bool _isRequired;
    bool _validate;
    QRegExp _validationRegExp;

    bool _isValid;
    QStringList _errMsgs;
};


class CPseudonymizerRules : public QHash<QString, CPseudonymizerRule> {
  public:
    CPseudonymizerRules();
    CPseudonymizerRules( const QString& rulesFileName );
    CPseudonymizerRules( const CPseudonymizerRules& other );
    CPseudonymizerRules& operator=( const CPseudonymizerRules& other );
    ~CPseudonymizerRules() { /* Nothing to do here. */ }

    int result() const { return _result; }

    QStringList fieldNames() const { return _fieldNames; }

    QStringList errorMessages() const { return _errMsgs; }

  protected:
    int _result;
    QStringList _fieldNames;
    QStringList _errMsgs;

    QCsv csvFromSpreadsheet( const QString& rulesFileName );
};

#endif // CPSEUDONYMIZERRULES_H
