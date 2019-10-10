#include "cerror.h"

CError::CError(){
  _type = Unspecified;
  _msg = QLatin1String("");
}


CError::CError( const ErrorType type, const QString& msg ) {
  _type = type;
  _msg = msg;
}


CError::CError( const CError& other ) {
  _type = other._type;
  _msg = other._msg;
}


CError& CError::operator=( const CError& other ) {
  _type = other._type;
  _msg = other._msg;

  return *this;
}


CErrorList::CErrorList(){
  // Nothing to do here.
}


void CErrorList::clear() {
  _list.clear();
}


int CErrorList::count() const {
  return _list.count();
}


CError CErrorList::itemAt( const int i ) const {
  return _list.at(i);
}


void CErrorList::append( const CError& err ) {
  _list.append( err );
}


void CErrorList::append( CError::ErrorType type, const QString& msg ) {
  _list.append( CError( type, msg ) );
}


void CErrorList::append( const CErrorList& src ) {
  for( int i = 0; i < src.count(); ++i )
    _list.append( src.itemAt(i) );
}


QString CErrorList::at( const int i ) {
  return _list.at(i).msg();
}


QString CErrorList::asText() {
  QString result;
  for( int i = 0; i < count(); ++i ) {
    result.append( _list.at(i).msg() );
    result.append( '\n' );
  }
  
  return result;
}


QString CError::typeAsString() {
  return typeAsString( type() );
}


QString CError::typeAsString( const ErrorType type ) {
  QString typeStr;

  switch( type ) {
    case CError::Information: typeStr = QStringLiteral("Information"); break;
    case CError::Question: typeStr = QStringLiteral("Question"); break;
    case CError::Warning: typeStr = QStringLiteral("Warning"); break;
    case CError::Critical: typeStr = QStringLiteral("Critical"); break;
    case CError::Fatal: typeStr = QStringLiteral("Fatal"); break;
    default:
      qDebug() << "Problem encountered in CError::typeAsString()";
      break;
  }

  return typeStr;
}
