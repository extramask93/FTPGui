#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "FTPClient.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void onLoginButtonClicked();
    void onLogoutButtonClicked();
    void onUpdateCurrentFolder();
    void onPrintTreeClicked();
    void onGetDirectories();
    void onDirectoryChanged(QListWidgetItem *);

private:
    Ui::MainWindow *ui;
    QString currentDir;
    QStringList folders;
    FTPClient ftp;
};

#endif // MAINWINDOW_H
