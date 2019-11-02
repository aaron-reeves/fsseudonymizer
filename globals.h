#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>

class CGlobals {
  public:
    static QString OrgName() { return QStringLiteral( "SRUC Epidemiology Research Unit" ); }
    static QString OrgDomain() { return QStringLiteral( "epidemiology.sruc.ac.uk" ); }
    static QString AppName() { return QStringLiteral( "%1 (FSS Edition)" ).arg( QStringLiteral(APP_NAME) ); }
    static QString AppVersion() { return QStringLiteral( APP_VERSION ); }

    static QString CopyrightInfo() { return QStringLiteral( "Copyright (C) 2019 Scotland's Rural College (SRUC)" ); }
    static QString LicenseStatement() { return QStringLiteral( "This is free software, released under the terms of the GNU General Public License" ); }
    static QString AppDescr() { return QStringLiteral( "Pseudonym generation from CSV and Microsoft Excel-formatted data sets" ); }

    static QString Author() { return QStringLiteral( "Aaron Reeves" ); }
    static QString EmailAddress() { return QStringLiteral( "Aaron.Reeves@sruc.ac.uk" ); }
    static QString Website() { return QStringLiteral( "https://epidemiology.sruc.ac.uk/projects/software/fss"); }

    static QString CopyrightNotice() { return QStringLiteral("%1.\n%2.\n\n%3.\n\n" ).arg( CopyrightInfo(), LicenseStatement(), AppDescr() ); }
};

#endif // GLOBALS_H
