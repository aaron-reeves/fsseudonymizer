#ifndef CBVDFIELD_H
#define CBVDFIELD_H

#include <QtCore>


class CBVDBaseField {
  public:
    CBVDBaseField( const QVariant::Type type = QVariant::Invalid );
    CBVDBaseField( const QString& name, const QVariant::Type type, const bool isRequired, const bool requiresEncryption );
    CBVDBaseField( const CBVDBaseField& other );
    CBVDBaseField& operator=( const CBVDBaseField& other );
    ~CBVDBaseField() { /* Do nothing */ }

    QString name();
    QVariant::Type expectedType();
    bool isRequired();
    bool requiresEncryption();

    void setIsRequired( const bool val ) { _isRequired = val; }
    void setRequiresEncryption( const bool val ) { _requiresEncryption = val; }

    bool fieldIsPresent();
    void setFieldIsPresent( const bool val );

  protected:
    QString _name;
    QVariant::Type _expectedType;
    bool _isPresent;
    bool _isRequired;
    bool _requiresEncryption;
};


class CBVDRecord;

class CBVDField : public CBVDBaseField {
  friend class CBVDRecord;
  friend class CBVDCoreFields;

  public:
    CBVDField( const QVariant::Type expectedType = QVariant::Invalid );
    CBVDField(
      const QString& fieldName,
      const QVariant::Type expectedType,
      const bool isRequired,
      const bool requiresEncryption
    );
    CBVDField( const CBVDField& other );
    CBVDField& operator=( const CBVDField& other );
    ~CBVDField() { /* Do nothing */ }

    QVariant::Type actualType() { return _data.type(); }

    QVariant data() { return _data; }
    bool isNull() { return _data.isNull(); }
    bool isValid();

    void debug();

  protected:
    void setData( const QVariant& data );
    void setData( const QVariant& data, const bool isValid );
    void setValid( const bool isValid ) { _isValid = isValid; }

    QVariant _data;
    bool _isValid;
};


#endif // CBVDFIELD_H
