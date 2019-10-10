#ifndef CBVRECORD_H
#define CBVRECORD_H

#include <QtCore>

#include <ar_general_purpose/csv.h>

#include "cerror.h"
#include "cbvdfield.h"



class CBVDData;

typedef QHash<QString, CBVDField*> CBVDFieldHash;

class CBVDRecord {
  public:
    CBVDRecord();
    CBVDRecord(QCsvObject* input, const int line, const QString& passphrase, const bool excludeParish );
    //CBVDRecord( const CBVDRecord& other ); // Use default copy constructor.
    //~CBVDRecord(); // Use default destructor.

    bool requirementsOK( const int lineID, CErrorList& msgs );
    bool isValid();
    CErrorList errMsgs() { return _errMsgs; }

    QString csvColumnHeaders( const bool includeIsValid, const bool excludeParish );
    QString csvText( const bool includeIsValid, const bool excludeParish );

    int csvResult() { return _csvProcessingResult; }

    bool contains( const QString& fieldName ) { return _fieldData.contains( fieldName ); }
    CBVDField* field( const QString& fieldName );

    // FIXME: Replace this function by simply checking valid_encrypted_postcode
    bool postcodeIsNull();

    // FIXME: Replace this function by simply checking valid_encrypted_cph
    bool herdIsNull();

    // FIXME: With changes mentioned above, this function should be obsolete.
    bool excludeParish() const { return _excludeParish; }

    static QString worseResult( QString testResult1, QString testResult2 );
    static bool isPcrTest( QString analysisType );

  protected:
    void initialize();

    void processCsvFields(QCsvObject* input, const int line, const QString& passphrase );
    void buildEncryptedField( const QString& canonicalName, const QString& inputName, QString data, const int lineID, const QString& passphrase );
    void postValidate( const int lineID );
    void postProcessBioBestValue( const int lineID );



    QString toCsvText( const QVariant& var );
    int _csvProcessingResult;

    CErrorList _errMsgs;

    // These are the "core" fields, most of which are required.
    // All of these are listed in bvdCoreFieldList.
    CBVDFieldHash _fieldData;

    bool _excludeParish;
};


class CBVDCoreFields {
  public:
    CBVDCoreFields();
    ~CBVDCoreFields();

    void addField(
      const QString& name,
      const QVariant::Type type,
      const bool isRequired,
      const bool requiresEncryption
    );

    void addFieldAlias( const QString& lookupName, const QString& canonicalName );

    int fieldCount() { return _list.count(); }
    int aliasCount() { return _hash.count(); }

    void clear();

    CBVDBaseField* at( const int i ) { return _list.at(i); }
    CBVDBaseField* at( const QString& name ) { return _hash.value( name ); }

    CBVDField buildField( const QString& inputName, const QString& canonicalName, QString data, const int lineID, CErrorList& errMsgs );

    QString findCanonicalName( const QString& lookupName );

    bool fieldHeadersOK( const QStringList& fieldsInFile, QStringList& missingFields );

    QVariant sampleType( const QVariant& key );
    QVariant analysisType( const QVariant& key );
    QVariant sampleResult( const QVariant& key );
    QVariant vpReference( const QVariant& key );
    QVariant herdStatus( const QVariant& key );
    QVariant herdType( const QVariant& key );
    QVariant vaccinationStatus( const QVariant& key );
    QVariant testOption( const QVariant& key );
    QVariant bvdHistory( const QVariant& key );
    bool isPooledIndicator( const QVariant& key );

  protected:
    void buildLookups();

    QList<CBVDBaseField*> _list;
    QHash<QString, CBVDBaseField*> _hash;
    QSet<QString> _canonicalNames;

    // Keys are always lower-case.
    QSet<QString> _pooledIndicators;
    QHash<QString, QString> _sampleTypes;
    QHash<QString, QString> _analysisTypes;
    QHash<QString, QString> _testResults;
    QHash<QString, QString> _vpReferences;
    QHash<QString, QString> _herdStatuses;
    QHash<QString, QString> _herdTypes;
    QHash<QString, QString> _vaccinationStatuses;
    QHash<QString, QString> _testOptions;
    QHash<QString, QString> _bvdHistories;

  private:
    Q_DISABLE_COPY( CBVDCoreFields )
};

extern CBVDCoreFields globalFieldList;

void buildGlobalFieldList();
void clearGlobalFieldList();



#endif // CBVRECORD_H
