
#include "cbvdfield.h"


CBVDBaseField::CBVDBaseField( const QVariant::Type type /* = QVariant::Invalid */ ) {
  _name = QString();
  _expectedType = type;
  _isPresent = false;
  _isRequired = false;
  _requiresEncryption = false;
}

CBVDBaseField::CBVDBaseField(
    const QString& name,
    const QVariant::Type type,
    const bool isRequired,
    const bool requiresEncryption
) {
  _name = name;
  _expectedType = type;
  _isPresent = false;
  _isRequired = isRequired;
  _requiresEncryption = requiresEncryption;
}

CBVDBaseField::CBVDBaseField( const CBVDBaseField& other ) {
  _name = other._name;
  _expectedType = other._expectedType;
  _isPresent = other._isPresent;
  _isRequired = other._isRequired;
  _requiresEncryption = other._requiresEncryption;
}

CBVDBaseField& CBVDBaseField::operator=( const CBVDBaseField& other ) {
  _name = other._name;
  _expectedType = other._expectedType;
  _isPresent = other._isPresent;
  _isRequired = other._isRequired;
  _requiresEncryption = other._requiresEncryption;

  return *this;
}


QString CBVDBaseField::name() { return _name; }
QVariant::Type CBVDBaseField::expectedType() { return _expectedType; }
bool CBVDBaseField::isRequired() { return _isRequired; }
bool CBVDBaseField::fieldIsPresent() { return _isPresent; }
void CBVDBaseField::setFieldIsPresent( const bool val ) { _isPresent = val; }
bool CBVDBaseField::requiresEncryption() { return _requiresEncryption; }

CBVDField::CBVDField( const QVariant::Type expectedType /* = QVariant::Invalid */ ) : CBVDBaseField( expectedType ) {
  _name = QString();
  _expectedType = expectedType;
  _data = QVariant( QVariant::String );
  _isValid = false;
  _isPresent = false;
  _isRequired = false;
  _requiresEncryption = false;
}

CBVDField::CBVDField(
    const QString& fieldName,
    const QVariant::Type expectedType,
    const bool isRequired,
    const bool requiresEncryption
) : CBVDBaseField( fieldName, expectedType, isRequired, requiresEncryption ) {
  _name = fieldName;
  _expectedType = expectedType;
  _data = QVariant( expectedType );
  _isValid = false;
  _isRequired = isRequired;
  _requiresEncryption = requiresEncryption;

  // These are currently ignored by CBVDField, but are held over from the base class anyway.
  _isPresent = false;
}


CBVDField::CBVDField( const CBVDField& other ) : CBVDBaseField( other ) {
  _name = other._name;
  _data = other._data;
  _expectedType = other._expectedType;
  _isValid = other._isValid;
  _isRequired = other._isRequired;
  _requiresEncryption = other._requiresEncryption;

  // These are currently ignored by CBVDField, but are held over from the base class anyway.
  _isPresent = other._isPresent;
}


CBVDField& CBVDField::operator=( const CBVDField& other ) {

  // The assignment operator from the base class has to be explicitly called.
  // See https://isocpp.org/wiki/faq/assignment-operators#assignment-op-in-derived-class
  CBVDBaseField::operator =( other );

  _name = other._name;
  _data = other._data;
  _expectedType = other._expectedType;
  _isValid = other._isValid;
  _isRequired = other._isRequired;
  _requiresEncryption = other._requiresEncryption;

  // These are currently ignored by CBVDField, but are held over from the base class anyway.
  _isPresent = other._isPresent;

  return *this;
}

void CBVDField::setData( const QVariant& data ) {
  //qDebug() << this->_name << "(1)";
  _data = data;
  _isPresent = true;
}

void CBVDField::setData( const QVariant& data, const bool isValid ){
  _data = data;
  _isValid = isValid;
  _isPresent = true;
}


bool CBVDField::isValid() {
  if( _isRequired )
    return( !_data.isNull() && ( actualType() == _expectedType ) && _isValid );
  else
    return( ( actualType() == _expectedType ) && _isValid );
}


void CBVDField::debug() {
  qDebug() << "--- BEGIN CBVDField::debug()";
  qDebug() << "name:" << _name;
  qDebug() << "types (exp, act):" << _expectedType << actualType() << ( actualType() == _expectedType ) << expectedType();
  qDebug() << "data:" << _data;
  qDebug() << "isPresent:" << _isPresent;
  qDebug() << "isRequired:" << _isRequired;
  qDebug() << "requiresEncryption:" << _requiresEncryption;
  qDebug() << "isValid:" << _isValid;
  qDebug() << "--- DONE CBVDField::debug()";
  qDebug();
}




