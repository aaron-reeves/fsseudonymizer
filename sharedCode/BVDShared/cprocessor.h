#ifndef CPROCESSOR_H
#define CPROCESSOR_H

#include <QtCore>

#include <BVDShared/cbvddata.h>

class CProcessor : public QObject {
  Q_OBJECT

  public:
    enum Result {
      SUCCESS = 0x0000,                  //    0
      BAD_COMMAND = 0x0001,              //    1
      INPUT_FILE_PROBLEM = 0x0002,       //    2
      DATA_VALIDATION_PROBLEM = 0x0004,  //    4
      OUTPUT_FILE_PROBLEM = 0x0008,      //    8
      ERROR_LOG_PROBLEM = 0x0010,        //   16
      ERROR_VARIANT_CONVERSION = 0x0020, //   32
      PROCESSING_INTERRUPTED = 0x0040,   //   64
      FAILED_DB_QUERY = 0x0080,          //  128
      BAD_CONFIG = 0x0100,               //  256
      FILE_SYSTEM_PROBLEM = 0x0200,      //  512
      REQUIRED_FIELDS_MISSING = 0x0400,  // 1024
      BAD_DATABASE = 0x0800,             // 2048
      FATAL_ERROR = 0x1000               // 4096
      //NEXT_PROBLEM = 0x2000
      //...
    };

    CProcessor( const bool enforceRequiredFields, QObject* parent = nullptr );
    CProcessor( const QHash<QString, QVariant>& params, const bool enforceRequiredFields, QObject* parent = nullptr );
    ~CProcessor();

    void setParams( const QHash<QString, QVariant>& params );
    bool fieldHeadersOK( const QStringList& fieldsInFile, QStringList& missingFields );
    int readAndEncryptData();

    bool silent();
    QString errors() { return _errMsgs.asText(); }
    int result() { return _result; }

  public slots:
    void interrupt();
    void bytesProcessed( const int val );
    void linesWritten( const int val );

  signals:
    void setProgressMax( int val );
    void setProgressValue( int val );
    void setProgressLabelText( QString val );
    void processInterrupted();

  protected:
    virtual void initialize( const bool enforceRequiredFields );

    virtual void validateParams();
    QFileInfo validateSharedParams();

    void resetResult();

    void readInputFile( CBVDData* bvdData );
    void writeEncryptedOutputFile( CBVDData* bvdData );
    void writeErrorLogFile();

    void handleError( CError::ErrorType type, const QString& msg );
    void handleError(const CErrorList& msgs );

    void debugParams();

    QHash<QString, QVariant> _params;
    int _result;
    CErrorList _errMsgs;
    int _totalBytesProcessed;
    bool _processingInterrupted;
    bool _enforceRequiredFields;
};

#endif // CPROCESSOR_H
