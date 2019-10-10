#ifndef CABOUTDIALOG_H
#define CABOUTDIALOG_H

#include <QDialog>

namespace Ui {
  class CAboutDialog;
}

class CAboutDialog : public QDialog {
  Q_OBJECT

  public:
    explicit CAboutDialog(QWidget *parent = nullptr);
    ~CAboutDialog();

  private:
    Ui::CAboutDialog *ui;
};

#endif // CABOUTDIALOG_H
