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

#ifndef CPROCESSOR_H
#define CPROCESSOR_H

#include <QtCore>

#include <ar_general_purpose/ctwodarray.h>

#include "cpseudonymizerrules.h"

class CProcessor {
  public:
    CProcessor( const QHash<QString, QString>& params );
    ~CProcessor() { /* Nothing to do here */ }

    int run();

    int result() const { return _result; }
    QStringList errorMessages() const { return _errMsgs; }

  protected:
    CTwoDArray<QVariant> getData( const QString& inputFileName );

    CPseudonymizerRules _rules;
    CTwoDArray<QVariant> _data;

    CTwoDArray<QVariant> _pseudonymizedData;

    int _result;
    QStringList _errMsgs;
    QHash<QString, QString> _params;


  private:
    Q_DISABLE_COPY( CProcessor )
};

#endif // CPROCESSOR_H
