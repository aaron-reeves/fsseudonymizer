/*
Fsseudonymizer - cprocessor.h/cpp
---------------------------------
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

class CProcessor : public QObject {
    Q_OBJECT

  public:
    enum CProcessorDataFormat {
      FormatUnspecified,
      FormatCSV,
      FormatExcel
    };

    CProcessor( const QHash<QString, QString>& params, bool useResourceFile, QObject* parent = nullptr );
    ~CProcessor();

    int readRules();
    int readData();

    int process();

    int writeOutput();

    int result() const { return _result; }

    static bool checkResourceForRules( bool& resourceOK, QHash<QString, QString>& params );

  signals:
    void stageStarted( const QString& stageDescr );
    void setStageSteps( const int nSteps );
    void setStageStepComplete( const int step );
    void stageComplete();

  protected slots:
    void slotSetStageSteps( const QString& unused, const int nSteps );

  protected:
    void getData( const QString& inputFileName );

    QStringList fileHeader();
    void writeOutputXlsx( const QString& outputFileName );
    void writeOutputCsv( const QString& outputFileName );

    CPseudonymizerRules* _rules;
    CTwoDArray<QVariant> _data;
    int _inputDataFormat;

    CTwoDArray<QVariant> _pseudonymizedData;

    int _result;
    QHash<QString, QString> _params;
    bool _useResourceFile;


  private:
    Q_DISABLE_COPY( CProcessor )
};

#endif // CPROCESSOR_H
