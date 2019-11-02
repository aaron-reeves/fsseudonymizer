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

    QString fieldName() const { return _fieldName; }
    bool pseudonymizeStandard() const { return _pseudonymizeStandard; }
    bool pseudonymizeSimplified() const { return _pseudonymizeSimplified; }
    bool removeField() const { return _remove; }
    bool isRequired() const { return _isRequired; }
    bool validate() const { return _validate; }
    QRegExp validationRegExp() const { return _validationRegExp; }

    bool isValid() const { return _isValid; }
    QStringList errorMessages() const { return _errMsgs; }

    QVariant process( const QVariant& val, const QString& passphrase, bool& error, QStringList& errMsgs ) const;

    void debug() const;

    static QString sha( const QString& value, const QString& passphrase );

  protected:
    void initialize();
    void assign( const CPseudonymizerRule& other );

    // This function MUST NOT change: doing so would break backward compatibility!
    QString removeAllSymbols( QString str ) const;

    int _rowNumber;

    QString _fieldName;
    bool _pseudonymizeStandard;
    bool _pseudonymizeSimplified;
    bool _remove;
    bool _isRequired;
    bool _validate;
    QRegExp _validationRegExp;

    bool _isValid;
    QStringList _errMsgs;
};


// Key is lowercase version of the field name
class CPseudonymizerRules : public QObject, public QHash<QString, CPseudonymizerRule> {
  Q_OBJECT

  public:
    CPseudonymizerRules( QObject* parent = nullptr );
//    CPseudonymizerRules( const QString& rulesFileName );
//    CPseudonymizerRules( const CPseudonymizerRules& other );
//    CPseudonymizerRules& operator=( const CPseudonymizerRules& other );
    ~CPseudonymizerRules() { /* Nothing to do here. */ }

    int readFile( const QString& rulesFileName, const bool readFromResource );

    int result() const { return _result; }

    QStringList fieldNames() const { return _fieldNames; }

    void debug() const;

  signals:
    void setStageSteps( const qint64 nSteps );
    void setStageStepComplete( const int step );

  protected slots:
    void slotSetStageSteps( const QString& unused, const int nSteps );

  protected:
    int _result;
    QStringList _fieldNames;

    QCsv csvFromSpreadsheet( const QString& rulesFileName, const CSpreadsheetWorkBook::SpreadsheetFileFormat format );

  private:
    Q_DISABLE_COPY( CPseudonymizerRules )
};

#endif // CPSEUDONYMIZERRULES_H
