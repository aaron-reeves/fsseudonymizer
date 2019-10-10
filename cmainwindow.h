#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>
#include <BVDShared/cprocessor.h>

namespace Ui {
  class CMainWindow;
}

class CMainWindow : public QMainWindow {
  Q_OBJECT

  public:
    explicit CMainWindow(QWidget *parent = 0);
    ~CMainWindow();

  protected slots:
    void updateParams();
    void updateParams( QString unused );
    void run();

    void about();

  protected:
    void initializeParams();
    void setControlsEnabled( QWidget* widget, const bool val );

    bool isTextFile();
    bool checkExtraneousQuotesOK();
    bool checkfileHeader();

    CProcessor* _processor;
    QHash<QString, QVariant> _params;

    QProgressDialog* _progress;

    bool processExtraneousQuotesFile();
    QString processExtraneousQuotesLine( QString line );
    bool _extraneousQuotesFileExists;
    QString _extraneousQuotesFile;

  private:
    Ui::CMainWindow *ui;
};

#endif // CMAINWINDOW_H
