#ifndef CBVDDATA_H
#define CBVDDATA_H

#include <QtCore>

#include "cbvdfield.h"
#include "cbvdrecord.h"


class CBVDData : public QObject, public QList<CBVDRecord*> {
  Q_OBJECT

  public:
    CBVDData( QObject* parent = nullptr );
    ~CBVDData();

    void addRecords( QCsvObject* input, const QString& passphrase, const bool excludeParish );
    bool writeOutput( const QString& outputPath, const QString& userName, const QString& userEmail, const bool excludeParish );

    CErrorList errMsgs() { return _errMsgs; }
    int result() { return _processingResult; }
    bool processInterrupted() { return _processInterrupted; }

    int nCommentRows(){ return _nCommentRows; }

  public slots:
    void interrupt();

  signals:
    void nBytesRead( int );
    void nLinesRead( int );
    void nLinesWritten( int );

  protected:
    int _processingResult;
    QCsvObject* _input;
    CErrorList _errMsgs;
    bool _processInterrupted;
    int _nFields;
    QString _passphrase;
    int _nCommentRows;
    bool _excludeParish;

    void addRecord( const int line );

    QString csvFileHeader( const QString& userName, const QString& userEmail );
};


#endif // CBVDDATA_H
