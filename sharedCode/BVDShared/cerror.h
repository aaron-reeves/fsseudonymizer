#include <QtCore>

class CError {
  public:
    enum ErrorType {
      Unspecified,
      Information,
      Question,
      Warning,
      Critical,
      Fatal
    };

    CError();
    CError( const ErrorType type, const QString& msg );
    CError( const CError& other );
    CError& operator=( const CError& other );
    ~CError() { /* Do nothing */ }

    ErrorType type() const { return _type; }
    QString msg() const { return _msg; }

    QString typeAsString();
    static QString typeAsString( const ErrorType type );

  protected:
    ErrorType _type;
    QString _msg;
};

class CErrorList {
  public:
    CErrorList();
    void clear();
    int count() const;
    QString at( const int i );
    CError itemAt( const int i ) const;
    void append( const CError& err );
    void append( const CError::ErrorType level, const QString& msg );
    void append( const CErrorList& src );
    QString asText();
    
  protected:
    QList<CError> _list;
};
