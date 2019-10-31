/*
XLPseudonymizer - cmainwindow.h/cpp
------------------------------------
Begin: 2019/10/13
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2019 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>

#include "cprocessor.h"

namespace Ui {
  class CMainWindow;
}

class CMainWindow : public QMainWindow {
  Q_OBJECT

  public:
    explicit CMainWindow(QWidget *parent = nullptr);
    ~CMainWindow() override;

  protected slots:
    void resourceWarning();

    void updateParams();
    void updateParams( const QString &unused );
    void process();

    void about();

  protected:
    void checkResourceForRules();
    void initializeParams();
    void setControlsEnabled( QWidget* widget, const bool val );

    void readSettings();
    void writeSettings();

    void showEvent( QShowEvent* event ) override;

    void closeEvent( QCloseEvent* event ) override;

    bool isTextFile();

    bool _useRulesFileFromResource;
    bool _resourceOK;
    QHash<QString, QString> _params;

  private:
    Ui::CMainWindow *ui;
};

#endif // CMAINWINDOW_H
