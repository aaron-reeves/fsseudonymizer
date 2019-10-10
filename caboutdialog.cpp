#include "caboutdialog.h"
#include "ui_caboutdialog.h"

CAboutDialog::CAboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CAboutDialog) {
  ui->setupUi(this);

  connect( ui->buttonBox, SIGNAL( accepted() ), this, SLOT( close() ) );

  ui->lblMailto->setText( QStringLiteral("<a href=\"mailto:aaron.reeves@sruc.ac.uk\">aaron.reeves@sruc.ac.uk</a>") );
  ui->lblMailto->setTextFormat(Qt::RichText);
  ui->lblMailto->setTextInteractionFlags(Qt::TextBrowserInteraction);
  ui->lblMailto->setOpenExternalLinks(true);

  this->setObjectName( QStringLiteral("me") );
  this->setAutoFillBackground( true );
  this->setStyleSheet(QStringLiteral("QWidget#me { background-color:#ffffff; }"));

  ui->lblAppName->setText( qApp->applicationName() );
  ui->lblAppVersion->setText( QStringLiteral( "Version %1" ).arg( qApp->applicationVersion() ) );
}


CAboutDialog::~CAboutDialog() {
  delete ui;
}
